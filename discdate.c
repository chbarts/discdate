#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define _GNU_SOURCE
#include <getopt.h>

/* Modified from https://rosettacode.org/wiki/Discordian_date */

#define day_of_week( x ) ((x) == 1 ? "Sweetmorn" :\
                          (x) == 2 ? "Boomtime" :\
                          (x) == 3 ? "Pungenday" :\
                          (x) == 4 ? "Prickle-Prickle" :\
                          "Setting Orange")

#define season( x ) ((x) == 0 ? "Chaos" :\
                    (x) == 1 ? "Discord" :\
                    (x) == 2 ? "Confusion" :\
                    (x) == 3 ? "Bureaucracy" :\
                    "The Aftermath")

#define date( x ) ((x)%73 == 0 ? 73 : (x)%73)

#define leap_year( x ) ((x) % 400 == 0 || (((x) % 4) == 0 && (x) % 100))

char *ddate(int y, int d)
{
    int dyear = 1166 + y;
    char *result = malloc(100 * sizeof(char));

    if (NULL == result) {
        fprintf(stderr, "Not enough RAM\n");
        exit(EXIT_FAILURE);
    }

    if (leap_year(y)) {
        if (d == 60) {
            sprintf(result, "St. Tib's Day, YOLD %d", dyear);
            return result;
        } else if (d >= 60) {
            --d;
        }
    }

    sprintf(result, "%s, %s %d, YOLD %d",
            day_of_week(d % 5), season(((d % 73) == 0 ? d - 1 : d) / 73),
            date(d), dyear);

    return result;
}

int months[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

int day_of_year(int y, int m, int d)
{
    for (; m > 1; m--) {
        d += months[m - 2];
        if (m == 3 && leap_year(y)) {
            ++d;
        }
    }

    return d;
}

void help(char name[])
{
    printf("%s [OPTION]...\n", name);
    puts("Display the Discordian date for a given date,");
    puts("with intelligent defaulting");
    puts("");
    puts("  -m, --month MONTH       month, from 1 (January) to 12 (December)");
    puts("  -d, --day DAY           day of month, from 1 to 28, 30, or 31)");
    puts("  -y, --year YEAR         year, from 1 to INT_MAX");
    puts("  -h, --help              print this help");
    puts("  -v, --version           print version information");
    puts("");
    puts("If an option is missing or zero, assumes current value.");
    puts("The year must be complete; two-digit years will (usually)");
    puts("not give the desired result.");
    puts("No range checking is done to determine if date falls within the");
    puts("period of a given calendar's use in any particular area,");
    puts("or if the date was skipped in any particular area.");
}

void version(void)
{
    puts("discdate version 0.1");
    puts("Copyright 2025, Chris Barts <chbarts@gmail.com>");
    puts("Licensed under the GNU General Public License, version 3.0 or, at your option, any later version.");
    puts("This software has NO WARRANTY, even for USABILITY or FITNESS FOR A PARTICULAR PURPOSE.");
}

int main(int argc, char *argv[])
{
    time_t now;
    struct tm *now_time;
    int doy, lind, c, m, d, y;
    char *result;
    struct option longopts[] = {
        { "month", 1, 0, 0 },
        { "day", 1, 0, 0 },
        { "year", 1, 0, 0 },
        { "help", 0, 0, 0 },
        { "version", 0, 0, 0 },
        { 0, 0, 0, 0 }
    };

    now = time(NULL);
    now_time = localtime(&now);
    doy = now_time->tm_yday + 1;
    y = now_time->tm_year + 1900;
    m = now_time->tm_mon + 1;
    d = now_time->tm_mday;

    if (1 == argc) {
        result = ddate(y, doy);
        puts(result);
        free(result);
        return 0;
    }

    while ((c =
            getopt_long(argc, argv, "m:d:y:hv", longopts, &lind)) != -1) {
        switch (c) {
        case 0:
            switch (lind) {
            case 0:
                m = atoi(optarg);
                break;
            case 1:
                d = atoi(optarg);
                break;
            case 2:
                y = atoi(optarg);
                break;
            case 3:
                help(argv[0]);
                return 0;
            case 4:
                version();
                return 0;
            default:
                help(argv[0]);
                exit(EXIT_FAILURE);
            }

            break;
        case 'm':
            m = atoi(optarg);
            break;
        case 'd':
            d = atoi(optarg);
            break;
        case 'y':
            y = atoi(optarg);
            break;
        case 'h':
            help(argv[0]);
            return 0;
        case 'v':
            version();
            return 0;
        default:
            help(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if ((m <= 0) || (d <= 0)) {
        fprintf(stderr,
                "discdate: Meaningless date year: %d month: %d day: %d\n",
                y, m, d);
        exit(EXIT_FAILURE);
    }

    if (2 == m) {
        if (leap_year(y)) {
            if (d > 29) {
                fprintf(stderr, "discdate: impossible date\n");
                exit(EXIT_FAILURE);
            }
    
        } else {
            if (d > 28) {
                fprintf(stderr, "discdate: impossible date\n");
                exit(EXIT_FAILURE);
            }
        }

    } else if (d > months[m - 1]) {
        fprintf(stderr, "discdate: impossible date\n");
        exit(EXIT_FAILURE);
    }

    doy = day_of_year(y, m, d);
    result = ddate(y, doy);
    puts(result);
    free(result);

    return 0;
}
