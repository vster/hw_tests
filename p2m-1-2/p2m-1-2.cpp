#include <iostream>
#include <stdio.h>
#include <termios.h>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <ctime>
#include "../util.h"

using namespace std;

#define MAXSIZE 100000000

static struct termios stored_settings;
char *program_name;

static u64bit tcode;
static long amemsize;
static size_t asize;
static u64bit dst[MAXSIZE];
static long long d_times;
static float delay=1;
static int test_mn;
static bool verbose;

void set_keypress(void)
{
    struct termios new_settings;
    tcgetattr(0,&stored_settings);
    new_settings = stored_settings;
    /* Disable canonical mode, and set buffer size to 1 byte */
    new_settings.c_lflag &= (~(ICANON|ECHO));
    new_settings.c_cc[VTIME] = 0;
    new_settings.c_cc[VMIN] = 1;
    tcsetattr(0,TCSANOW,&new_settings);
    return;
}

void reset_keypress(void)
{
    tcsetattr(0,TCSANOW,&stored_settings);
    return;
}

int get_one_char(void)
{
    set_keypress();
    char c=getchar();
    reset_keypress();
    return c;
}

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

void usage(void)
{
    cout << "*****************************************************************************\n";
    cout << "Usage: \n";
    cout << program_name << " [-s <size>] [-t <template>] [-m <number>] [-d <delay>] [-v]\n";
    cout << "\nDescription and default parameters:\n";
    cout << "-s - array size (bytes) = 104857600 = 100 Mb\n";
    cout << "-t - template (64-bit hex code in big-endian order) = ffffffffffffffff\n";
    cout << "-m - test method number = 1\n";
    cout << "     Possible values of m:\n";
    cout << "     1 - constant template elements in arrays;\n";
    cout << "     2 - interleaving of one template element and one zero element in arrays;\n";
    cout << "     3 - interleaving of template array and zero array.\n";
    cout << "-d - delay between arrays (millions empty cycles) = 1\n";
    cout << "     0 - no delay;\n";
    cout << "     1 - one million empty cycles (if CPU Clock = 2GHz, delay is about 2.5 ms);\n";
    cout << "     or any other positive real number.\n";
    cout << "-v - show dots \".\" on display when test runs\n";
    cout << "-h - help\n";
    cout << "*****************************************************************************\n";
    cout << "If you start this program without options it works with default parameters.\n";
    cout << "To change parameters you must press \"q\" to quit from program and restart\n";
    cout << "it by entering command with necessary options after terminal prompt.\n";
    cout << "\nExamples:\n";
    cout << "\nSet array size to 20 Mb = 20971520 bytes.\n";
    cout << program_name << " -s 20971520\n";
    cout << "\nSet template of elements to hex-code 0000000000000000\n";
    cout << program_name << " -t 0\n";
    cout << "\nSet template of elements to hex-code 5555555555555555\n";
    cout << program_name << " -t 5555555555555555\n";
    cout << "\nSet array size to 20 Mb and template to hex-code 5555555555555555\n";
    cout << program_name << " -s 20971520 -t 5555555555555555\n";
    cout << "*****************************************************************************\n";
}

void show_params()
{
    cout << "\nArray size = " << dec << amemsize << " bytes" << " = "
            << (float)amemsize/(1024*1024) << " Mb" << endl;
    cout << "Template (hex code) = " << hex << setw(16) << setfill('0')
           << tcode << endl;
    cout << "Delay = " << dec << delay << endl;
    cout << "Test method = " << dec << test_mn << endl;
    cout << "Show dots - ";
    if (verbose)
        cout << "on";
    else
        cout << "off";
    cout << endl;
}

void p2m_u64bit_1 (void)
{
    register u64bit tcr;
    tcr=tcode;

    for (size_t n=0; n<asize; n++)
        dst[n]=tcr;
}

void p2m_u64bit_2 (void)
{
    register u64bit tcr1=tcode;
    register u64bit tcr2=0x0;

    for (size_t n=0; n<asize/2; n++)
    {
        dst[2*n]=tcr1;
        dst[2*n+1]=tcr2;
    }
}

void p2m_u64bit_3 (void)
{
    register u64bit tcr1=tcode;
    register u64bit tcr2=0x0;
    // register long long nr=0;

    for (size_t n=0; n<asize; n++)
    {
        dst[n]=tcr1;
    }

    // Delay - small prosechka
    for (int i=0;i< d_times/2;i++);

    // Zero array - less 2 times
    for (size_t n=0; n<asize/2; n++)
    {
        dst[n]=tcr2;
    }
}

void end_test(void)
{
    reset_keypress();
    cout << "\n*** End test *** \n";
    cout << "Bye-bye!\n";
    exit(0);
}

int main(int argc, char *argv[])
{
    int rez=0;
    tcode = 0xffffffffffffffff;
    amemsize = 104857600;
    verbose = false;
    test_mn = 1;
    delay=1;
    int ch;

    program_name = argv[0];

    cout << "Processor-to-memory copy test. 64-bit. Version 1.2\n";

    while ( (rez = getopt(argc,argv,"s:t:m:d:v?h")) != -1){
        switch (rez){
        case 's':
            amemsize = strtol(optarg,NULL,10);
            break;
        case 't':
            tcode = strtoul(optarg,NULL,16);
            break;
        case 'm':
            test_mn = atoi(optarg);
            break;
        case 'd':
            delay = atof(optarg);
            break;
        case 'v':
            verbose = true;
            break;
        case 'h':
        default:
             usage();
             exit(0);
        };
    };

    asize = amemsize/8;
    d_times=(long long)delay*1000000;

    show_params();

    // One time for debug
    switch (test_mn)
    {
        case 1: p2m_u64bit_1(); break;
        case 2: p2m_u64bit_2(); break;
        case 3: p2m_u64bit_3(); break;
    }

    set_keypress();

    while (true)
    {
      cout << "\n*** Pause ***\n";
      cout << "Processor-to-memory copy test.\n";
      cout << "Press key:\n";
      cout << "   \"r\" - run test \n";
      cout << "   \"m\" - set test method number \n";
      cout << "   \"d\" - set value of delay\n";
      cout << "   \"v\" - on/off verify test runs - show dots \".\"\n";
      cout << "   \"p\" - show current parameters\n";
      cout << "   \"h\" - help\n";
      cout << "   \"q\" - quit\n";
      ch=getchar();
      switch (ch)
      {
        case 'r':
            cout << "\n*** Run test *** \nPress any key for pause.\n";
            while(!kbhit())
            {
                switch (test_mn)
                {
                    case 1: p2m_u64bit_1(); break;
                    case 2: p2m_u64bit_2(); break;
                    case 3: p2m_u64bit_3(); break;
                }

                // Delay - prosechka
                for (int i=0;i< d_times;i++);

                if (verbose)
                    cout << ".";
            }
            ch=getchar();
            if (ch != 'q')
                break;
        case 'q':
            end_test();
        case 'h':
            usage();
            break;
        case 'p':
            show_params();
            break;
        case 'm':
            cout << "\nEnter test method number: ";
            reset_keypress();
            int tmp;
            cin >> tmp;
            set_keypress();
            if (tmp > 0 && tmp < 4 )
            {
                test_mn = tmp;
                cout << "\nTest method number = " << test_mn << endl;
            }
            else
                cout << "\nInvalid value\n";
            getchar();
            break;
        case 'd':
            cout << "\nEnter value of delay: ";
            reset_keypress();
            float ftmp;
            cin >> ftmp;
            set_keypress();
            if (ftmp >= 0)
            {
                delay = ftmp;
                d_times = 1000000*delay;
                cout << "\nDelay = " << delay << endl;
            }
            else
                  cout << "\nInvalid value\n";
            getchar();
            break;
        case 'v':
            if (verbose)
            {
                cout << "\nDon't show dots when test runs\n";
                verbose = false;
            }
            else if (!verbose)
            {
                cout << "\nShow dots when test runs\n";
                verbose = true;
            }
            break;
        default:
            // cout << "\nIncorrect key\n";
            break;
       }
    }

    reset_keypress();
    return 0;
}


