#include "i18n_error.h"
#include "i18n.h"
#include <string.h>
#include <errno.h>

const char *i18n_error(int error)
{
  switch (error) {
  case EACCES:
    return i18n->errno_eacces;
  case EAGAIN:
    return i18n->errno_eagain;
  case EDQUOT:
    return i18n->errno_edquot;
  case EEXIST:
    return i18n->errno_eexist;
  case EINVAL:
    return i18n->errno_einval;
  case ENOENT:
    return i18n->errno_enoent;
  default:
    return strerror(error);
  }
}
