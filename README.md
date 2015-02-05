# wol
Wake on Lan Command Line Client

This is a simple command line Wake on Lan Client.   Its used to wake up machines from sleep mode.  I typically use it to
wake up my home machine when I'm on the road so I can remote into it without having to leave it on 24x7.  This saves lots
of $$ in power.

# Usage
'''
Wake on Lan (c)2010 Mycal.net
USAGE: wol -p pw [-h <help>] mac 
         mac --<mac address>        Using the following format xx:xx:xx:xx:xx:xx.  Must be specified, must be last.
         pw  --<password>           Optional Wake on lan password xx:xx:xx:xx:xx:xx
         -v  --<verbose>            Verbose Output
         -h  --<help>               print usage of the function
'''

# Example

wakeme is an example script:
'''
# wake the machine with the mac 54:5A:5B:FA:E6:C8
wol 54:5A:5B:FA:E6:C8
'''

# Building
Using the makefile in the src directory should built on most linux platforms.   There is no make file for win32, but it
will build as a console app with no issues.



