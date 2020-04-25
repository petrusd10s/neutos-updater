#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#define AMS_URL     "https://api.github.com/repos/borntohonk/NEUTOS/releases"

int downloadFile(const char *url, const char *output, int api_mode);

#endif