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


static bool initialized = false;
static pthread_t thread;
static int socketFd = -1;

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

typedef struct {
    char* command;
    void (*function)(char*, char**, int*);
    char* helpMsg;
} UdpServerCommand;

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

void UdpServer_init(AdcBuffer* pBuffer)
{
    if (initialized) return;

    int res = pthread_create(&thread, NULL, UdpServer_run, pBuffer);
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

        // TODO: Send reply.
        sinLen = sizeof(sinRemote);
        if (reply) {
            int res2 = sendto(socketFd, reply, replyLen, 0, (struct sockaddr*) &sinRemote, sinLen);
            if (res2 == -1) {
                SYS_WARN("sendto failed.\n");
            }
            free(reply);
        }
        else {
            SYS_WARN("reply = NULL.\n");
        }
    }

    return NULL;
}

static void UdpServer_processCommand(char* command, char** outReply, int* outReplyLen)
{
    assert(command);

    const int maxNumTokens = 2;
    char* tokens[maxNumTokens];
    char* arg1 = strtok(command, UDP_SERVER_COMMAND_SEP);
    if (!arg1) {
        // TODO: Set reply to help message and return.
    }
    char* arg2 = strtok(NULL, UDP_SERVER_COMMAND_SEP);

    for (int i = 0; i < NUM_ELEMS(supportedCommands); i++) {
        char str[UDP_SERVER_MAX_MSG_LEN];
        snprintf(str, UDP_SERVER_MAX_MSG_LEN, "%s\n", supportedCommands[i].command);
        if (strncmp(str, arg1, UDP_SERVER_MAX_MSG_LEN) == 0) {
            LOG(LOG_LEVEL_DEBUG, "Received valid command: '%s'.\n", str);
            (*supportedCommands[i].function)(arg2, outReply, outReplyLen);
            break;
        }
    }
}

static void UdpServer_help(char* arg2, char** outReply, int* outReplyLen)
{
    LOG(LOG_LEVEL_DEBUG, "Entered.\n");
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
    // TODO: Implement function.
    int x = 1;
}
static void UdpServer_length(char* arg2, char** outReply, int* outReplyLen)
{
    // TODO: Implement function.
    int x = 1;
}
static void UdpServer_history(char* arg2, char** outReply, int* outReplyLen)
{
    // TODO: Implement function.
    int x = 1;
}
static void UdpServer_get(char* arg2, char** outReply, int* outReplyLen)
{
    // TODO: Implement function.
    int x = 1;
}
static void UdpServer_dips(char* arg2, char** outReply, int* outReplyLen)
{
    // TODO: Implement function.
    int x = 1;
}
static void UdpServer_stop(char* arg2, char** outReply, int* outReplyLen)
{
    // TODO: Implement function.
    int x = 1;
}
static void UdpServer_repeatPrevCommand(char* arg2, char** outReply, int* outReplyLen)
{
    // TODO: Implement function.
    int x = 1;
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

    initialized = false;
}