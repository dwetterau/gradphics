#include <iostream>
#include <time.h>
#include <stdarg.h>

#include <assert.h>

#include "CommandLineUI.h"
#include "../fileio/bitmap.h"

#include "../RayTracer.h"

using namespace std;

// ***********************************************************
// from getopt.cpp
// it should be put in an include file.
//
extern int getopt(int argc, char **argv, char *optstring);
extern char* optarg;
extern int optind, opterr, optopt;
// ***********************************************************

// The command line UI simply parses out all the arguments off
// the command line and stores them locally.
CommandLineUI::CommandLineUI( int argc, char** argv )
	: TraceUI()
{
	int i;

	progName=argv[0];
  m_accelerated = true;
  objectLimit = 3;
  depthLimit = 15;
  m_cm = false;
  m_stoc = false;
  cutoff = 0.0;
  while( (i = getopt( argc, argv, (char*)"tr:w:h:a:n:d:l:")) != EOF )
	{
		switch( i )
		{
            case 'd':
                depthLimit = atoi(optarg);
                break;
            case 'l':
                objectLimit = atoi(optarg);
                break;
			case 'r':
				m_nDepth = atoi( optarg );
				break;
      case 'a':
        m_nAa = atoi( optarg );
        break;
			case 'w':
				m_nSize = atoi( optarg );
				break;
      case 'n':
        m_accelerated = false;
        break;
			default:
			// Oops; unknown argument
			std::cerr << "Invalid argument: '" << i << "'." << std::endl;
			usage();
			exit(1);
		}
	}

	if( optind >= argc-1 )
	{
		std::cerr << "no input and/or output name." << std::endl;
		exit(1);
	}

	rayName = argv[optind];
	imgName = argv[optind+1];
}

int CommandLineUI::run()
{
	assert( raytracer != 0 );
	raytracer->loadScene( rayName );

	if( raytracer->sceneLoaded() )
	{
		int width = m_nSize;
		int height = (int)(width / raytracer->aspectRatio() + 0.5);

		raytracer->traceSetup( width, height );

		clock_t start, end;
		start = clock();

		for( int j = 0; j < height; ++j )
			for( int i = 0; i < width; ++i )
				raytracer->tracePixel(i,j);

		end=clock();

		// save image
		unsigned char* buf;

		raytracer->getBuffer(buf, width, height);

		if (buf)
			writeBMP(imgName, width, height, buf);

		double t=(double)(end-start)/CLOCKS_PER_SEC;
		std::cout << "total time = " << t << " seconds" << std::endl;
        return 0;
	}
	else
	{
		std::cerr << "Unable to load ray file '" << rayName << "'" << std::endl;
		return( 1 );
	}
}

void CommandLineUI::alert( const string& msg )
{
	std::cerr << msg << std::endl;
}

void CommandLineUI::usage()
{
	std::cerr << "usage: " << progName << " [options] [input.ray output.bmp]" << std::endl;
	std::cerr << "  -r <#>      set recursion level (default " << m_nDepth << ")" << std::endl; 
	std::cerr << "  -w <#>      set output image width (default " << m_nSize << ")" << std::endl;
	std::cerr << "  -a <#>      set aa for the output image (default 1 x 1)" << std::endl;
	std::cerr << "  -n          don't use any Kd trees" << std::endl;
}
