/**
 * @brief Enables own debug messages, which can be disabled.
 *
 * @author Copyright (C) René Herthel <rene.herthel@haw-hamburg.de>
 */

#ifndef DEBUG_H
#define DEBUG_H

/** Set to 0 to disable Debug messages */
#define MSG_EN     (0)

/**
 * @brief Defined debug message, prints if debug is enabled via makefile
 */
#if MSG_EN
#    define DMSG(fmt, args...) fprintf(stderr, fmt, ## args)
#else
#    ifndef DMSG
#        undef DMSG /**< Just to be sure */
#        define DMSG(fmt, args...) /**< Do nothing */
#    endif
#endif

#endif /* DEBUG_H */
