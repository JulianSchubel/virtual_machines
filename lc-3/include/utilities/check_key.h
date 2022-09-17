#ifndef CHECK_KEY_H
#define CHECK_KEY_H

uint16_t check_key();

uint16_t check_key()
{
    /* Structure type representing a file descriptor set for select() */
    /* readfds is the set of file descriptors to be tested to determine if input is possible */
    fd_set readfds;
    /* Initialize the file descriptor set to zero  */
    FD_ZERO(&readfds);

    /* Adds STDIN_FILNO file descriptor to the readfds file descriptor set */
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    /* select(): blocks until one or more of a set of file descriptors becomes ready */
    /* A file descriptor is considered ready if it is possible to perform a corresponding I/O operation */
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

#endif //CHECK_KEY_H
