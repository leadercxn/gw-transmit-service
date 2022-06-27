#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include <mqtt.h>
#include "posix_sockets.h"

#include "trace.h"

static uint8_t  m_tx_buff[256] = "\0";
static uint16_t m_tx_len;

/* publish */
static struct mqtt_client   m_publisher_client;
static int                  m_publisher_sockfd = -1;
static pthread_t            m_pushlish_pthread_id;

static uint8_t              m_pub_sendbuf[1024];
static uint8_t              m_pub_recvbuf[1024];

/* subscribe */
static struct mqtt_client   m_subscribe_client;
static int                  m_subscribe_sockfd = -1;
static pthread_t            m_subscribe_pthread_id;

static uint8_t              m_sub_sendbuf[1024];
static uint8_t              m_sub_recvbuf[1024];


static void exit_thread(int status, int sockfd, pthread_t *p_thread_id)
{
    if (sockfd != -1) 
    {
        close(sockfd);
    }

    if (p_thread_id != NULL)
    {
        pthread_cancel(*p_thread_id);
    }

    exit(status);
}

static void subscribe_rece_cb(void** unused, struct mqtt_response_publish *published)
{
    /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
    char* topic_name = (char*) malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    trace_debug("Received publish('%s'): %s\n", topic_name, (const char*) published->application_message);

    free(topic_name);
}

static void* publish_client_refresher(void* client)
{
    while(1)
    {
        mqtt_sync((struct mqtt_client*) client);
        usleep(100000U);
    }
    return NULL;
}

static void* subscribe_client_refresher(void* client)
{
    while(1)
    {
        mqtt_sync((struct mqtt_client*) client);
        usleep(100000U);
    }
    return NULL;
}

static void usage( void )
{
    trace_printf("Available options:\n\n" );
    trace_printf(" -h       print this help\n" );
    trace_printf(" -ip      mqtt broker ip address, default 42.194.206.95\n" );
    trace_printf(" -p       mqtt port, default 1883\n" );
    trace_printf(" -ptop    mqtt session publisher topic, default publish\n" );
    trace_printf(" -stop    mqtt session subscribe topic, default subscribe\n" );
    trace_printf(" -txt     mqtt session publisher tx txt, default \"1234\"\n" );
    trace_printf(" -bin     mqtt session publisher tx bin, default \"0x1234\"\n" );
    trace_printf(" -i       mqtt session publisher tx interval, default 1000 ms\n" );
    trace_printf(" -rp      mqtt session publisher tx repetition, -1 loop until stopped, default -1\n" );

    trace_printf("\n" );
}

int main(int argc, const char *argv[])
{
    char ip_addr[32] = "42.194.206.95";
    char port[16] = "1883";
    char publish_topic[64] = "publish";
    char subscribe_topic[64] = "subscribe";

    uint8_t count = 0;

    //Prase the cmd
    for(uint8_t i = 1; i < argc; )
    {
        if( strcmp( argv[i], "-h" ) == 0 )
        {
            i++;
            usage();
            return 0;
        }
        else if( strcmp( argv[i], "-ip" ) == 0 )
        {
            i++;
            count = sscanf( argv[i], "%s", ip_addr);
            if( count != 1 )
            {
                trace_error( "error invail param %s\r\n", argv[i]);
                return 0;
            }
            i++;
        }
        else if( strcmp( argv[i], "-p" ) == 0 )
        {
            i++;
            count = sscanf( argv[i], "%s", port);
            if( count != 1 )
            {
                trace_error( "error invail param %s\r\n", argv[i]);
                return 0;
            }
            i++;
        }
        else if( strcmp( argv[i], "-ptop") == 0 )
        {
            i++;
            count = sscanf( argv[i], "%s", publish_topic);
            if( count != 1 )
            {
                trace_error( "error invail param %s\r\n", argv[i]);
                return 0;
            }
            i++;
        }
        else if( strcmp( argv[i], "-stop") == 0 )
        {
            i++;
            count = sscanf( argv[i], "%s", subscribe_topic);
            if( count != 1 )
            {
                trace_error( "error invail param %s\r\n", argv[i]);
                return 0;
            }
            i++;
        }
        else if( strcmp( argv[i], "-txt" ) == 0 )
        {
            i++;

            m_tx_len = strlen(argv[i]);
            if(m_tx_len > 256)
            {
                trace_error( "error: tx buf too long\r\n" );
                return 0;
            }

            if(m_tx_len != 0)
            {
                strcpy((char *)m_tx_buff, argv[i]);
            }

            i++;
        }
        else
        {
            trace_error( "error: invalid arg :%s\r\n", argv[i]);
            return 0;
        }
    }

    trace_printf("\n");
    trace_info("ip_addr             :%s\n",     ip_addr);
    trace_info("port                :%s\n",     port);
    trace_info("publish_topic       :%s\n",     publish_topic);
    trace_info("subscribe_topic     :%s\n",     subscribe_topic);
    trace_info("publish data len    :%u\n",     m_tx_len);


    /* open the non-blocking TCP socket (connecting to the broker) */
    m_publisher_sockfd = open_nb_socket(ip_addr, port);
    if (m_publisher_sockfd == -1)
    {
        trace_error("Failed to open socket: \n");
        exit_thread(EXIT_FAILURE, m_publisher_sockfd, NULL);
    }

    m_subscribe_sockfd = open_nb_socket(ip_addr, port);
    if (m_subscribe_sockfd == -1)
    {
        trace_error("Failed to open socket: \n");
        exit_thread(EXIT_FAILURE, m_subscribe_sockfd, NULL);
    }

    /* publish mqtt client */
    mqtt_init(&m_publisher_client, m_publisher_sockfd, m_pub_sendbuf, sizeof(m_pub_sendbuf), 
               m_pub_recvbuf, sizeof(m_pub_recvbuf), NULL);

    /* subscribe mqtt client */
    mqtt_init(&m_subscribe_client, m_subscribe_sockfd, m_sub_sendbuf, sizeof(m_sub_sendbuf), 
               m_sub_recvbuf, sizeof(m_sub_recvbuf), subscribe_rece_cb);

    /* Ensure we have a clean session */
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;

    /* Send connection request to the broker. */
    mqtt_connect(&m_publisher_client, NULL, NULL, NULL, 0, NULL, NULL, connect_flags, 400);

    /* Send connection request to the broker. */
    mqtt_connect(&m_subscribe_client, NULL, NULL, NULL, 0, NULL, NULL, connect_flags, 400);

    if (m_publisher_client.error != MQTT_OK)
    {
        fprintf(stderr, "error: %s\n", mqtt_error_str(m_publisher_client.error));
        exit_thread(EXIT_FAILURE, m_publisher_sockfd, NULL);
    }

    if (m_subscribe_client.error != MQTT_OK)
    {
        fprintf(stderr, "error: %s\n", mqtt_error_str(m_subscribe_client.error));
        exit_thread(EXIT_FAILURE, m_subscribe_sockfd, NULL);
    }


    if(pthread_create(&m_pushlish_pthread_id, NULL, publish_client_refresher, &m_publisher_client)) 
    {
        fprintf(stderr, "Failed to start publish thread\n");
        exit_thread(EXIT_FAILURE, m_publisher_sockfd, NULL);
    }

    if(pthread_create(&m_subscribe_pthread_id, NULL, subscribe_client_refresher, &m_subscribe_client)) 
    {
        fprintf(stderr, "Failed to start subscribe thread\n");
        exit_thread(EXIT_FAILURE, m_subscribe_sockfd, NULL);
    }

    /* subscribe */
    mqtt_subscribe(&m_subscribe_client, subscribe_topic, 0);

    /* start publishing the time */
    trace_info("%s is ready to publish, Press ENTER to tx msg\n", argv[0]);
    trace_info("Press CTRL-D (or any other key) to exit.\n\n");

    while(fgetc(stdin) == '\n') //阻塞式等待输入
    {
#if 0
        /* get the current time */
        time_t timer;
        time(&timer);
        struct tm* tm_info = localtime(&timer);
        char timebuf[26];
        strftime(timebuf, 26, "%Y-%m-%d %H:%M:%S", tm_info);
#endif

        trace_info("%s published : \"%s\" \n", argv[0], m_tx_buff);

        /* publish the time */
        mqtt_publish(&m_publisher_client, publish_topic, m_tx_buff, strlen((char *)m_tx_buff) + 1, MQTT_PUBLISH_QOS_0);

        /* check for errors */
        if (m_publisher_client.error != MQTT_OK)
        {
            fprintf(stderr, "error: %s\n", mqtt_error_str(m_publisher_client.error));
            exit_thread(EXIT_FAILURE, m_publisher_sockfd, &m_pushlish_pthread_id);
        }
    }

    /* disconnect */
    trace_printf("\n");
    trace_info("%s disconnecting from %s\n", argv[0], ip_addr);
    sleep(1);

    /* exit */
    exit_thread(EXIT_SUCCESS, m_publisher_sockfd, &m_pushlish_pthread_id);
}
