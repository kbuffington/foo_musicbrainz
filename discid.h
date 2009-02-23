#ifndef __DISCID_H__
#define __DISCID_H__

char *get_discid(int tracks_count, unsigned int *tracks_lengths, int pregap);
unsigned char *rfc822_binary (void *src,unsigned long srcl,unsigned long *len);

#endif
