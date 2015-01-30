# sstrings: a strings(1) replacement

After reading that GNU binutils' strings(1) utility is affected by a
buffer overflow vulnerability and finding out that a simple tool like
that is more complex than it should be (at least in my opinion) I
decided to brush up my C skills on UNIX-like systems and to write
another implementation of strings(1). Enter sstrings(1).

sstrings(1) is a Simpler strings(1) (or Stupid strings) that does just
what I suppose strings(1) should do: read chunks of binary data from a
file or from stdin and output every sequence of 4 or more printable
characters ending either with a '\0' or a '\n' to stdout.

I will follow the [Open Group specifications](http://pubs.opengroup.org/onlinepubs/9699919799/utilities/strings.html)
as close as I can and you're free to nag me when something's wrong but I
warn you: this tool is mostly written for my own amusement and it's not
meant to be as good or as complete as GNU strings(1).

For istance it ignores the `-a` flag and ALWAYS scans the whole
file. Since this behaviour is implementation dependent I think I'm
allowed to do it.

I'm also not planning to make this utility universally portable, my aim
is to have it run on a i386 personal computer running OpenBSD and then
see if it runs on Linux too. Probably I will kindly ask you to test it
on different UNIX-like operating systems, different architectures and
different compilers and then send me some feedback about it but that's
not mandatory.

The recommended compiler is GCC 4 and the following line should output
no warnings:

    c99 -Wall -Wextra -pedantic sstrings.c -o sstrings

The license is the MIT license and you can read it in `LICENSE`. It
boils down to: do what you want with this as long as you give me credit
for writing it in the first place and please don't sue me.

