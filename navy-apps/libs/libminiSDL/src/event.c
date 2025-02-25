#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *event) {
    char buf[64];
    char type[8];
    char key[16];
    if (NDL_PollEvent(buf, sizeof(buf))) {
      sscanf(buf,"%s %s", type, key);
      // printf("receive event: %s, type=%s, key=%s\n", buf, type, key);
      if(strcmp(type, "kd") == 0) {
        event->type = SDL_KEYDOWN;
      } else {
        event->type = SDL_KEYUP;
      }
      for(int i = 0; i < sizeof(keyname); i ++) {
        if(strcmp(keyname[i], key) == 0) {
          event->key.keysym.sym = i;
          // printf("event:%d, keyname:%s\n", i, keyname[i]);
          return 1;
        }
      }
    }
    return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  while (1) {
    if(SDL_PollEvent(event))
      return 1;
  }
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
