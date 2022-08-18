#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include <mqtt.h>
#include "posix_sockets.h"

#include "boards.h"
#include "version.h"

#include "app_files.h"
#include "app_data.h"
#include "config_handler.h"

#include "trace.h"


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

static void app_threads_init(void)
{

}


int main(int argc, const char *argv[])
{
    uint8_t count = 0;
    int err_code = 0;

    //Prase the cmd
    for(uint8_t i = 1; i < argc; )
    {
        if( strcmp( argv[i], "-h" ) == 0 )
        {
            i++;
            usage();
            return 0;
        }
        else
        {
            trace_error( "error: invalid arg :%s\r\n", argv[i]);
            return 0;
        }
    }
    trace_printf("\n");

    err_code = app_config_data_get();
    if(err_code < 0)
    {
        trace_errorln("app_config_data_get %d", err_code);
    }



    // 线程初始化
    app_threads_init();


    return 0;
}


