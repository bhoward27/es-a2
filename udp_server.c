#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "udp_server.h"
#include "shutdown.h"
#include "log.h"
#include "utils.h"
#include "adc_stats.h"

#define TERMINATION_MSG "Terminating program...\n"

typedef struct {
    char* command;
    void (*function)(char*, char**, int*);
    char* helpMsg;
} UdpServerCommand;

static void* UdpServer_run(void* args);
static void UdpServer_processCommand(char* command, char** outReply, int* outReplyLen);
static void UdpServer_help(char* arg2, char** outReply, int* outReplyLen);
static void UdpServer_count(char* arg2, char** outReply, int* outReplyLen);
static void UdpServer_length(char* arg2, char** outReply, int* outReplyLen);
static void UdpServer_history(char* arg2, char** outReply, int* outReplyLen);
static void UdpServer_get(char* arg2, char** outReply, int* outReplyLen);
static void UdpServer_dips(char* arg2, char** outReply, int* outReplyLen);
static void UdpServer_stop(char* arg2, char** outReply, int* outReplyLen);
static void UdpServer_repeatPrevCommand(char* arg2, char** outReply, int* outReplyLen);

static const UdpServerCommand supportedCommands[] = {
    {"help", UdpServer_help, "display this help message"},
    {"count", UdpServer_count, "display the total number of samples taken"},
    {"length", UdpServer_length, "display number of samples in history (both max, and current)"},
    {"history", UdpServer_history, "display the full sample history being saved"},
    {"get", UdpServer_get, "display the 10 most recent history values"},
    {"dips", UdpServer_dips, "display the number of dips"},
    {"stop", UdpServer_stop, "stop the server"},
    {"", UdpServer_repeatPrevCommand, "repeat last command"}
};

static bool initialized = false;
static pthread_t thread;
static int socketFd = -1;
static AdcBuffer* pBuffer = NULL;
static UdpServerCommand lastCommand = supportedCommands[0];
static char lastArg2[UDP_SERVER_MAX_MSG_LEN];

void UdpServer_init(AdcBuffer* pBufferArg)
{
    if (initialized) return;

    pBuffer = pBufferArg;
    int res = pthread_create(&thread, NULL, UdpServer_run, NULL);
    if (res != 0) {
        SYS_DIE("pthread_create failed.\n");
    }

    initialized = true;
}

static void* UdpServer_run(void* args)
{
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(UDP_SERVER_PORT);
    socketFd = socket(PF_INET, SOCK_DGRAM, 0);
    if (socketFd == -1) {
        SYS_DIE("Failed to create socket.\n");
    }
    int res = bind(socketFd, (struct sockaddr*) &sin, sizeof(sin));
    if (res == -1) {
        SYS_DIE("Failed to bind to socket with file descriptor = %d.\n", socketFd);
    }

    lastArg2[0] = '\0';

    while (!isShutdownRequested()) {
        struct sockaddr_in sinRemote;
        unsigned int sinLen = sizeof(sinRemote);
        char messageRx[UDP_SERVER_MAX_MSG_LEN];
        int bytesRx = recvfrom(socketFd,
                               messageRx,
                               UDP_SERVER_MAX_MSG_LEN - 1,
                               0,
                               (struct sockaddr*) &sinRemote,
                               &sinLen);
        if (bytesRx == -1) {
            SYS_WARN("recvfrom() failed.\n");
            continue;
        }

        messageRx[bytesRx] = '\0';

        char* reply = NULL;
        int replyLen = 0;
        UdpServer_processCommand(messageRx, &reply, &replyLen);

        // Send reply.
        if (reply) {
            if (replyLen <= UDP_SERVER_MAX_MSG_LEN) {
                sinLen = sizeof(sinRemote);
                int res2 = sendto(socketFd, reply, replyLen * sizeof(reply[0]), 0, (struct sockaddr*) &sinRemote, sinLen);
                if (res2 == -1) {
                    SYS_WARN("sendto failed.\n");
                }
            }
            else {
                // Send the message in parts. netcat seems to have a limited buffer size for a single message, so don't
                // want to exceed that.
                int i = 0;
                int bytesToSend = replyLen;
                for (; bytesToSend > UDP_SERVER_MAX_MSG_LEN; bytesToSend -= UDP_SERVER_MAX_MSG_LEN,
                     i += UDP_SERVER_MAX_MSG_LEN) {

                    sinLen = sizeof(sinRemote);
                    int res2 = sendto(socketFd, reply + i, UDP_SERVER_MAX_MSG_LEN * sizeof(reply[0]), 0, (struct sockaddr*) &sinRemote, sinLen);
                    if (res2 == -1) {
                        SYS_WARN("sendto failed.\n");
                    }

                }
                if (bytesToSend > 0) {
                    sinLen = sizeof(sinRemote);
                    int res3 = sendto(socketFd, reply + i, bytesToSend * sizeof(reply[0]), 0, (struct sockaddr*) &sinRemote, sinLen);
                    if (res3 == -1) {
                        SYS_WARN("sendto failed.\n");
                    }
                }
            }

            free(reply);
        }
        else {
            LOG(LOG_LEVEL_WARN, "reply = NULL.\n");
        }
    }

    return NULL;
}

static void UdpServer_processCommand(char* command, char** outReply, int* outReplyLen)
{
    assert(command);

    char* arg1 = strtok(command, UDP_SERVER_COMMAND_SEP);
    if (!arg1) {
        UdpServer_help(NULL, outReply, outReplyLen);
    }
    char* arg2 = strtok(NULL, UDP_SERVER_COMMAND_SEP);

    for (int i = 0; i < NUM_ELEMS(supportedCommands); i++) {
        char str[UDP_SERVER_MAX_MSG_LEN];
        snprintf(str, UDP_SERVER_MAX_MSG_LEN, "%s\n", supportedCommands[i].command);
        if (strncmp(str, arg1, UDP_SERVER_MAX_MSG_LEN) == 0 ||
            strncmp(supportedCommands[i].command, arg1, UDP_SERVER_MAX_MSG_LEN) == 0) {

            LOG(LOG_LEVEL_DEBUG, "Received valid command: '%s'.\n", str);
            (*supportedCommands[i].function)(arg2, outReply, outReplyLen);
            lastCommand = (strncmp(supportedCommands[i].command, "", UDP_SERVER_MAX_MSG_LEN) == 0) ?
                                                                                    lastCommand : supportedCommands[i];
            if (arg2) {
                strncpy(lastArg2, arg2, UDP_SERVER_MAX_MSG_LEN - 1);
                lastArg2[UDP_SERVER_MAX_MSG_LEN - 1] = '\0';
            }
            break;
        }
    }
}

static void UdpServer_help(char* arg2, char** outReply, int* outReplyLen)
{
    const int maxHelpMsgLen = 1024;
    const int maxBodyLen = NUM_ELEMS(supportedCommands) * maxHelpMsgLen;
    char* helpMsgBody = malloc(sizeof(helpMsgBody[0]) * maxBodyLen);
    if (!helpMsgBody) {
        SYS_DIE("malloc failed!\n");
    }
    helpMsgBody[0] = '\0';
    for (int i = 0; i < NUM_ELEMS(supportedCommands); i++) {
        char helpMsg[maxHelpMsgLen];
        if (strncmp(supportedCommands[i].command, "get", maxHelpMsgLen) == 0) {
            snprintf(helpMsg, maxHelpMsgLen, "%s -- %s\n", "get 10", supportedCommands[i].helpMsg);
        }
        else if (strncmp(supportedCommands[i].command, "", maxHelpMsgLen) == 0) {
            snprintf(helpMsg, maxHelpMsgLen, "%s -- %s\n", "<enter>", supportedCommands[i].helpMsg);
        }
        else {
            snprintf(helpMsg, maxHelpMsgLen, "%s -- %s\n", supportedCommands[i].command, supportedCommands[i].helpMsg);
        }
        strncat(helpMsgBody, helpMsg, maxHelpMsgLen - 1);
    }
    *outReplyLen = strnlen(helpMsgBody, maxBodyLen);
    *outReply = helpMsgBody;
}

static void UdpServer_count(char* arg2, char** outReply, int* outReplyLen)
{
    uint64 count = AdcBuffer_getTotalNumSamplesTaken(pBuffer);
    const int maxLen = 1024;
    char* countMsg = malloc(sizeof(countMsg[0]) * maxLen);
    if (!countMsg) {
        SYS_DIE("malloc failed!\n");
    }
    snprintf(countMsg, maxLen, "Number of samples taken = %llu.\n", count);
    *outReplyLen = strnlen(countMsg, maxLen);
    *outReply = countMsg;
}
static void UdpServer_length(char* arg2, char** outReply, int* outReplyLen)
{
    uint64 size = AdcBuffer_getSize(pBuffer);
    uint64 maxSize = AdcBuffer_getMaxSize(pBuffer);
    const int maxLen = 2048;
    char* lengthMsg = malloc(sizeof(lengthMsg[0]) * maxLen);
    if (!lengthMsg) {
        SYS_DIE("malloc failed!\n");
    }
    snprintf(lengthMsg, maxLen, "History can hold\t%llu samples.\nCurrently holding\t%llu samples.\n", maxSize, size);
    *outReplyLen = strnlen(lengthMsg, maxLen);
    *outReply = lengthMsg;
}

static void UdpServer_history(char* arg2, char** outReply, int* outReplyLen)
{
    uint64 n = 0;
    adc_in* samples = AdcBuffer_getSamples(pBuffer, &n);
    const int numCharsPerSample = 7;
    const int numSamplesPerLine = 20;
    const int numFullLines = (n / numSamplesPerLine);
    const int numSamplesInLastNonFullLine = n % 20;
    const int numCharsForFullLines = numFullLines * (numSamplesPerLine * numCharsPerSample + 1); // +1 for \n
    const int numCharsForNonFullLine = numSamplesInLastNonFullLine * numCharsPerSample +
                                       ((numSamplesInLastNonFullLine == 0) ? 0 : 1); //+1 for \n

    const int len = numCharsForFullLines + numCharsForNonFullLine + 1; // + 1 for \0.
    char* historyMsg = malloc(sizeof(historyMsg[0]) * len);
    if (!historyMsg) {
        SYS_DIE("malloc failed!\n");
    }
    historyMsg[0] = '\0';

    for (int i = 0; i < n; i++) {
        volt sampleV = Adc_convertToVolts(samples[i]);
        char s[numCharsPerSample + 1];
        snprintf(s, numCharsPerSample + 1, "%.3f, ", sampleV);
        strncat(historyMsg, s, numCharsPerSample);
        if (i % numSamplesPerLine == numSamplesPerLine - 1) {
            strcat(historyMsg, "\n");
        }
    }
    // This gave me a compiler warning and not sure how to fix...
    // strncat(historyMsg, "\n", 1);
    // So just going to use strtcat.
    if (n % numSamplesPerLine != 0) {
        strcat(historyMsg, "\n");
    }

    *outReplyLen = len;
    *outReply = historyMsg;

    free(samples);
}
static void UdpServer_get(char* arg2, char** outReply, int* outReplyLen)
{
    uint64 n = 0;
    adc_in* samples = AdcBuffer_getSamples(pBuffer, &n);
    if (!arg2) {
        free(samples);
        LOG(LOG_LEVEL_DEBUG, "arg2 = NULL. Caused by bad user input from netcat?\n");
        return;
    }
    int k = atoi(arg2);
    if (k < 0 || k > n) {
        char* errMsg = malloc(UDP_SERVER_MAX_MSG_LEN * sizeof(errMsg[0]));
        if (!errMsg) {
            SYS_DIE("malloc failed!\n");
        }
        snprintf(errMsg, UDP_SERVER_MAX_MSG_LEN, "Invalid input '%d'. Current valid range is [0, %llu].\n", k, n);

        *outReplyLen = strnlen(errMsg, UDP_SERVER_MAX_MSG_LEN);
        *outReply = errMsg;

        free(samples);

        return;
    }

    const int numCharsPerSample = 7;
    const int numSamplesPerLine = 20;
    const int numFullLines = (k / numSamplesPerLine);
    const int numSamplesInLastNonFullLine = k % 20;
    const int numCharsForFullLines = numFullLines * (numSamplesPerLine * numCharsPerSample + 1); // +1 for \n
    const int numCharsForNonFullLine = numSamplesInLastNonFullLine * numCharsPerSample +
                                       ((numSamplesInLastNonFullLine == 0) ? 0 : 1); //+1 for \n

    const int len = numCharsForFullLines + numCharsForNonFullLine + 2; // + 1 for \0 and + 1 to be safe.
    char* historyMsg = malloc(sizeof(historyMsg[0]) * len);
    if (!historyMsg) {
        SYS_DIE("malloc failed!\n");
    }
    historyMsg[0] = '\0';

    // Show the k most recent samples.
    for (int i = n - k; i < n; i++) {
        volt sampleV = Adc_convertToVolts(samples[i]);
        char s[numCharsPerSample + 1];
        snprintf(s, numCharsPerSample + 1, "%.3f, ", sampleV);
        strncat(historyMsg, s, numCharsPerSample);
        if ((i - n + k) % numSamplesPerLine == numSamplesPerLine - 1) {
            strcat(historyMsg, "\n");
        }
    }

    if (k % numSamplesPerLine != 0) {
        strcat(historyMsg, "\n");
    }

    *outReplyLen = strnlen(historyMsg, len);
    *outReply = historyMsg;

    free(samples);
}
static void UdpServer_dips(char* arg2, char** outReply, int* outReplyLen)
{
    uint64 numSamples = 0;
    adc_in* samples = AdcBuffer_getSamples(pBuffer, &numSamples);
    double meanBrightness = AdcBuffer_getCurrentMean(pBuffer);
    uint64 numDips =  AdcStats_dips(samples, numSamples, meanBrightness);
    free(samples);
    char* dipsMsg = malloc(sizeof(dipsMsg[0]) * UDP_SERVER_MAX_MSG_LEN);
    if (!dipsMsg) {
        SYS_DIE("malloc failed!\n");
    }
    snprintf(dipsMsg, UDP_SERVER_MAX_MSG_LEN, "# Dips = %llu.\n", numDips);

    *outReply = dipsMsg;
    *outReplyLen = strnlen(dipsMsg, UDP_SERVER_MAX_MSG_LEN);
}
static void UdpServer_stop(char* arg2, char** outReply, int* outReplyLen)
{
    int len = strlen(TERMINATION_MSG) + 1;
    char* stopMsg = malloc(len * sizeof(char));
    if (!stopMsg) {
        SYS_DIE("malloc failed!\n");
    }
    snprintf(stopMsg, len, TERMINATION_MSG);
    *outReply = stopMsg;
    *outReplyLen = len;

    requestShutdown();
}
static void UdpServer_repeatPrevCommand(char* arg2, char** outReply, int* outReplyLen)
{
    lastCommand.function(lastArg2, outReply, outReplyLen);
}

void UdpServer_waitForShutdown(void)
{
    int res = pthread_join(thread, NULL);
    if (res != 0) {
        SYS_WARN("pthread_join failed.\n");
    }

    int closeRes = close(socketFd);
    if (closeRes == -1) {
        SYS_WARN("Failed to close socket with file descriptor = %d.\n", socketFd);
    }

    pBuffer = NULL;
    initialized = false;
}