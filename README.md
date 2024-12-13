# a40
40-character alphabet text encoding, 3in2 format. 16bits=3characters, true random access fixed encoding not variable encoding. case insensitive. letters, numbers, and 4 symbols (usually space, period, comma, apostrophe).



This is useful for comments in headerfiles like storing 42character comment in only 28bytes, or using an unsigned short / 2byte of file format header to store any valid 3character file extension (just right pad it with space for 1 or 2 character extensions).

because only 64,000 (in decimal) codepoints are used (40^3) then we are having some room within the 16bit (65,536 in decimal) to spread things out, which enables us to say that: no encoded value will have any nulls in it (neither the first byte of the 16bit value, nor the second byte). This means you can still null-terminate an a40 encoded string rather than having to length prefix it or use fixed-length buffers. But the null terminator would probably be a 16bit null as you are probably storing it as an array of shorts, but if you are storing it as an array of chars and then just "eating" 2 chars at a time then it can be null terminated, which means you can do strcpy and strlen and so on on it as it will be a valid c-str. Expanding it in this way reduces performance a little bit and also uses up a bit of address space in the current implementation which makes it not good for microcontrollers.


Pull requests on a bit twiddling magic to replace the 2x 64kb lookup tables (precalculated on demand and stored in empty .bss arrays) are welcome.

