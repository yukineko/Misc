#include <stdio.h>
#include <event.h>
#include <event2/event-config.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 16
#define BTN_EVENT_SYS_PATH "/sys/class/gpio/gpio23/value"
int btn_value = 0;
typedef struct timespec timesp;

timesp pre_time;
int long_push = 0;
int long_pushed(timesp* pre, timesp* now ){
    long nano_diff = (now->tv_nsec - pre->tv_nsec) / (1000 * 1000);
    //    printf("%ld %ld\n", pre->tv_sec, (pre->tv_nsec) / (1000 * 1000));
    //    printf("%ld %ld\n", now->tv_sec, (now->tv_nsec) / (1000 * 1000));
    if(nano_diff < 200 && nano_diff > 0){
	if(now->tv_sec - pre->tv_sec < 1){
	    return 0;
	}
    }
    return 1;
}


void btn_read(int fd, short event, void* arg){
    int len;
    char buf[BUF_SIZE];
    struct event *ev = arg;

    lseek(fd, 0, SEEK_SET);
    len = read(fd, buf, sizeof(buf) - 1);
    if(len == -1){
        warn("can not read");
    } else if(len == 0){
	warnx("close");
    } else {
	buf[len] = '\0';
	int new_btn_value = atoi(buf);
	if(btn_value != new_btn_value){
	    btn_value = new_btn_value;
	    if(btn_value == 1){
		clock_gettime(CLOCK_REALTIME, &pre_time);
		printf("button pushed\n");
	    } else {
		timesp now_time;
		clock_gettime(CLOCK_REALTIME, &now_time);	    
		int ret = long_pushed(&pre_time, &now_time);
		if(ret == 0){
		    printf("%s\n", "short push\n");
		} else {
		    printf("button release\n");
		}
		long_push = 0;
	    }
	} else if(btn_value == 1 && new_btn_value == 1){
	    timesp now_time;
	    clock_gettime(CLOCK_REALTIME, &now_time);	    
	    int ret = long_pushed(&pre_time, &now_time);
	    if(long_push == 0 && ret != 0){
		printf("%s", "long push start\n");
		long_push = 1;
	    }
	}
    }
    event_add(ev, NULL);
}


int event_initialize(int fd){
    struct event evfile;
    printf("%s\n", "event initialize");

    event_init();
    event_set(&evfile, fd, EV_READ, btn_read, &evfile);
    event_add(&evfile, NULL);
    event_dispatch();
    return 0;
}


int main(){
    int fd = open(BTN_EVENT_SYS_PATH, O_RDONLY);
    if(fd == -1){
	err(EXIT_FAILURE, "can not open btn gpio file. %s", BTN_EVENT_SYS_PATH);
    }
    event_initialize(fd);
    return 0;
}
