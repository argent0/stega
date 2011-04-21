
#define WATCH_SIZE_T( var ) printf(#var"=%lu\n",(unsigned long) var )
#define WATCH_NL printf("\n")
#define WATCH_STR( var ) printf(#var"=%s\n",var )

#define printfBuffer( counter, buf, len, fmt )\
	for ( counter = 0; counter < len; counter = counter + 1 )\
		printf(fmt" ",buf[counter])
