#include <unistd.h>

#include "utils.h"
#include <stdio.h>
#include "args.h"
#include "web.h"


int main(int argc, char *argv[]) {
	struct gengetopt_args_info args;

	if (cmdline_parser(argc, argv, &args) != 0) {
		ERROR(1, "Error parsing command line");
		return 1;
	}

	char *gecko = args.gecko_arg;
	char *firefox = args.firefox_arg;

	printf("------\n");
	printf("%s\n", gecko);
	printf("%s\n", firefox);
	printf("------\n");


	web_context ctx = web_init(gecko, firefox);

	web_open(ctx, "https://www.example.com");

	sleep(5);

	web_close(&ctx);

}
