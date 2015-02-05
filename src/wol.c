
/*!                                                            www.mycal.net			
 *---------------------------------------------------------------------------
 *! \file wol.c
 *  \brief Wake on lan command line software
 *																			
 *---------------------------------------------------------------------------
 * Version                                                                  -
 *		0.1 Original Version June 3, 2006									-        
 *
 *---------------------------------------------------------------------------    
 * Version                                                                  -
 * 0.1 Original Version August 31, 2006                                     -
 *																			-
 * (c)2006 mycal.net. All Rights Reserved									-
 *---------------------------------------------------------------------------
 *
 */

/*					*/ 
/* include files 	*/
/*					*/ 
#include "mytypes.h"
#include "config.h"
#include "debug.h"

//
// network_init()-
// If anything needs to be initialized before using the network, put it here, mostly this
//	is for windows.
//
S16
network_init()
{
#if defined(WIN32) || defined(WINCE)

	WSADATA w;								/* Used to open Windows connection */
	/* Open windows connection */
	if (WSAStartup(0x0101, &w) != 0)
	{
		printf( "Winsock Init Failed.\n");
		exit(1);
	}	

#endif
return(0);
}
//
// Send the wakeup packet as a UDP packet to port 7 broadcast onl local network
//
// mac, converted mac address string, 6 bytes long
// pw,  password string of 6 bytes, or null if no password is to be used
//
//
int
wake_up(U8 *mac, U8 *pw)
{
	SOCKET	soc;
	struct sockaddr_in	client;		/* Information about the client */
    int broadcast = 1;
	char	buffer[1024];
	int		i,j;
	int		ret=-1;


	//
	// Get a UDP socket
	//
	soc = socket(AF_INET, SOCK_DGRAM, 0);

	if (soc == INVALID_SOCKET)
	{
		DEBUG1("Could not create socket.\n");
		return(-1);
	}

	// this call is what allows broadcast packets to be sent:
	if (setsockopt(soc, SOL_SOCKET, SO_BROADCAST, (char *)&broadcast, sizeof broadcast) != -1) 
	{
		// fill the first 6 bytes with 0xff
		memset((void *)&buffer,0xff,6);
		// Build WOL packet with 16 repititions of mac address (needs to be 17 to account for the aboce 6)
		for(i=6;i<(17*6);i++)
			buffer[i]=mac[i%6];

		// Check for pw
		if(pw)
		{
			for(j=i;j<i+6;j++)
				buffer[j]=pw[j%6];
			i=j;
		}
		// successful setup of broadcast, lets sendthe packet
		// clear out socket structure
		memset((void *)&client, '\0', sizeof(struct sockaddr_in));
		client.sin_family = AF_INET;					// host byte order
		client.sin_port =  htons(7);					// Try to use this port  htons? +++
		/* Set server address */
		client.sin_addr.s_addr= 0xffffffff;				// broadcast address
		//
 	    //should we send it twice?
		//
		ret=sendto(soc, (char *)buffer, i, 0, (struct sockaddr *)&client, sizeof(struct sockaddr));
		if(ret==i)
			ret=0;
		else
			ret=-1;


	}
	else
	{
		ret=-1;
	}
	
	closesocket(soc);

	return(ret);
}



int
get_mac(U8 *str, U8 *mac)
{
U8		*subst;
int		i=0,ret=1;


	// scan out the bytes.
	subst=(U8 *) strtok((char *) str,": \n");

	if(NULL!=subst)
	{
		// Loop for 6 bytes
		for(i=0;i<6;i++)
		{
			mac[i]=	(U8)strtol((char *) subst,(char **)NULL, 16);
			subst= (U8 *) strtok(NULL,": \n");
			if(NULL==subst)
				break;
		}
	}
	if(i==5)
		ret=0;

	return(ret);
}




// Specify usage of the program
static void usage(int argc, char *argv[])
{
	printf("Wake on Lan (c)2006 Mycal.net\n");
	printf("USAGE: wol -p pw [-h <help>] mac \n");
	printf("\t mac --<mac address>        Using the following format xx:xx:xx:xx:xx:xx.  Must be specified, must be last.\n");
	printf("\t pw  --<password>           Optional Wake on lan password xx:xx:xx:xx:xx:xx\n");
	printf("\t -v  --<verbose>            Verbose Output\n");	
	printf("\t -h  --<help>               print usage of the function\n");	
}




int main(int argc, char *argv[])
{
	U16			i,go=0;
	int			verbose=0;
	int			pw_set=0;
	
	U8		mac[12];
	U8		pw[12];

	//------------------------------------------------------------------
	// Argument Scan command line args, first for -h
	//------------------------------------------------------------------


	for(i=1;i<argc;i++)
	{
        	if(0==strncmp(argv[i],"-h",2))
		{
			go=0;
			break;
		}
		else if(0==strncmp(argv[i],"-v",2))
		{
			verbose=1;
			continue;
		}
		else if(0==strncmp(argv[i],"-p",2))
		{
			if(get_mac((U8*)argv[++i],pw))
			{
				if(verbose) printf("Failed to parse password correctly.\n\n");
				go=0;
				break;
			}
			pw_set=1;
			continue;
		}
		if('-'!=argv[i][0])
		{
			// parse out mac
			if(get_mac((U8*)argv[i],mac))
			{
				go=0;
				break;
			}
			go=1;
			continue;
		}
		else
		{
			go=0;
			break;
		}
	}

	if(go)
	{
		//
		//
		//
		network_init();

		if(verbose)
		{
			printf("Sending out Magic Packet to %.2x:%.2x:%.2x:%.2x:%.2x:%.2x.\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

			if(pw_set)
			{
				printf("  Using Password %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",pw[0],pw[1],pw[2],pw[3],pw[4],pw[5]);
			}
		}

		if(pw_set)
		{	
			if(-1==wake_up(mac, pw))
			{
				if(verbose) printf("Magic Packet send failed.\n");
			}
			else
			{
				if(verbose) printf("Magic Packet Sent.\n");
			}
		}
		else
		{
			if(-1==wake_up(mac, 0))
			{
				if(verbose) printf("Magic Packet send failed.\n");
			}
			else
			{
				if(verbose) printf("Magic Packet Sent.\n");
			}
		}
	}
	else
	{
		usage(argc,argv);
		exit(0);
	}

	return(0);
}



