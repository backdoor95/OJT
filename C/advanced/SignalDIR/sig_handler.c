void sig_handler(int sig, void (*handler)(int))
{
    struct sigaction action;
    memset(&action, 0x00, sizeof(struct sigaction));

    action.sa_handler = handler;
    sigemptyset(&(action.sa_mask));
    sigaddset(&(action.sa_mask),sig);
    action.sa_flags = 0;
    sigaction(sig, &action, 0);

    return;
}
