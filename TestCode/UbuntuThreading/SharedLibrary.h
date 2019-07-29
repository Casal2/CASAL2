#ifndef SOURCE_SHAREDLIBRARY_H_
#define SOURCE_SHAREDLIBRARY_H_

#ifdef __cplusplus
extern "C" {
#endif

#define EXPORT __attribute__((visibility("default")))

int EXPORT Run();

#ifdef __cplusplus
}
#endif /* _cplusplus */
#endif /* SOURCE_SHAREDLIBRARY_H_ */
