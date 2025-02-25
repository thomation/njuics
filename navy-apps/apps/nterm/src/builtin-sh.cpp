#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>
char * envp[1];
char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}
static char * argv[10];
static void sh_handle_cmd(const char *cmd) {
  // printf("cmd: %s, len:%d\n", cmd, strlen(cmd));
  if(strcmp(cmd, "exit\n") == 0){
    exit(0);
  } else if(strcmp(cmd, "bird\n") == 0) {
    argv[0] = "bird";
    execve("bird", argv, envp);
  }
  printf("exec error:%s\n", cmd);
}

void builtin_sh_run() {
  char *path = getenv("PATH");
  envp[0] = path;
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
