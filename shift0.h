
#include <features.h>
#include <sys/types.h>

__BEGIN_DECLS

struct cont {};

int32_t reset(int32_t (*fun)());
int32_t shift0(int32_t (*fun)(struct cont*));
int32_t runcont(struct cont *fun, int32_t val);

__END_DECLS

