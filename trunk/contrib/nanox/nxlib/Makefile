all:		rgbtxt.h

rgbtxt.h:	rgb.txt rgbtxt.pl
		sort -f -k4d rgb.txt | perl rgbtxt.pl > rgbtxt.h
