#include "hw.h"
#include "print.h"
#include "main.h"
#include "version.h"
#include "options.h"

#include <unistd.h>
#include <stdio.h>

static char *id = "@(#) $Id$";

void usage(const char *progname)
{
  fprintf(stderr, "Hardware Lister (lshw) - %s\n", getpackageversion());
  fprintf(stderr, "usage: %s [-format] [-options ...]\n", progname);
  fprintf(stderr, "       %s -version\n", progname);
  fprintf(stderr, "\n");
  fprintf(stderr, "\t-version        print program version\n");
  fprintf(stderr, "\nformat can be\n");
  fprintf(stderr, "\t-html           output hardware tree as HTML\n");
  fprintf(stderr, "\t-xml            output hardware tree as XML\n");
  fprintf(stderr, "\t-short          output hardware paths\n");
  fprintf(stderr, "\noptions can be\n");
  fprintf(stderr,
	  "\t-class CLASS    only show a certain class of hardware\n");
  fprintf(stderr, "\t-C CLASS        same as '-class CLASS'\n");
  fprintf(stderr,
	  "\t-disable TEST   disable a test (like pci, isapnp, cpuid, etc. )\n");
  fprintf(stderr,
	  "\t-enable TEST    enable a test (like pci, isapnp, cpuid, etc. )\n");
  fprintf(stderr, "\n");
}

void status(const char *message)
{
  static size_t lastlen = 0;
                                                                                
  if (isatty(2))
  {
    fprintf(stderr, "\r");
    for (size_t i = 0; i < lastlen; i++)
      fprintf(stderr, " ");
    fprintf(stderr, "\r%s\r", message);
    fflush(stderr);
    lastlen = strlen(message);
  }
}

int main(int argc,
	 char **argv)
{
  bool htmloutput = false;
  bool xmloutput = false;
  bool hwpath = false;

  disable("isapnp");
#ifndef DEBUG
  //disable("usb");
#endif

  if (!parse_options(argc, argv))
  {
    usage(argv[0]);
    exit(1);
  }

  if (argc == 2)
  {
    if (strcmp(argv[1], "-version") == 0)
    {
      printf("%s\n", getpackageversion());
      exit(0);
    }
    if (strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "--help") == 0)
    {
      usage(argv[0]);
      exit(0);
    }
    if (strcmp(argv[1], "-xml") == 0)
      xmloutput = true;

    if (strcmp(argv[1], "-html") == 0)
      htmloutput = true;

    if (strcmp(argv[1], "-short") == 0)
      hwpath = true;

    if (!xmloutput && !htmloutput && !hwpath)
    {
      usage(argv[0]);
      exit(1);
    }
  }

  if (argc > 2)
  {
    usage(argv[0]);
    exit(1);
  }

  if (geteuid() != 0)
  {
    fprintf(stderr, "WARNING: you should run this program as super-user.\n");
  }

  {
    hwNode computer("computer",
		    hw::system);

    scan_system(computer);

    if (hwpath)
      printhwpath(computer);
    else
    {
      if (xmloutput)
	printxml(computer);
      else
	print(computer, htmloutput);
    }
  }

  (void) &id;			// avoid warning "id defined but not used"

  return 0;
}