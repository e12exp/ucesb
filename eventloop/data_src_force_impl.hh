
#ifndef __DATA_SRC_FORCE_IMPL_H__
#define __DATA_SRC_FORCE_IMPL_H__

#define FORCE_IMPL_FCN(returns,fcn_name,...) \
  template returns fcn_name(__VA_ARGS__);

#if defined(USE_LMD_INPUT) || defined(USE_HLD_INPUT) || defined(USE_RIDF_INPUT)

#define FORCE_IMPL_DATA_SRC_FCN(returns,fcn_name) \
  FORCE_IMPL_FCN(returns,fcn_name,__data_src<0,0> &__buffer); \
  FORCE_IMPL_FCN(returns,fcn_name,__data_src<0,1> &__buffer); \
  FORCE_IMPL_FCN(returns,fcn_name,__data_src<1,0> &__buffer); \
  FORCE_IMPL_FCN(returns,fcn_name,__data_src<1,1> &__buffer);

#define FORCE_IMPL_DATA_SRC_FCN_ARG(returns,fcn_name,...) \
  FORCE_IMPL_FCN(returns,fcn_name,__data_src<0,0> &__buffer,__VA_ARGS__); \
  FORCE_IMPL_FCN(returns,fcn_name,__data_src<0,1> &__buffer,__VA_ARGS__); \
  FORCE_IMPL_FCN(returns,fcn_name,__data_src<1,0> &__buffer,__VA_ARGS__); \
  FORCE_IMPL_FCN(returns,fcn_name,__data_src<1,1> &__buffer,__VA_ARGS__);

#define FORCE_IMPL_DATA_SRC_FCN_HDR(returns,fcn_name) \
  FORCE_IMPL_FCN(returns,fcn_name,subevent_header *__header,__data_src<0,0> &__buffer); \
  FORCE_IMPL_FCN(returns,fcn_name,subevent_header *__header,__data_src<0,1> &__buffer); \
  FORCE_IMPL_FCN(returns,fcn_name,subevent_header *__header,__data_src<1,0> &__buffer); \
  FORCE_IMPL_FCN(returns,fcn_name,subevent_header *__header,__data_src<1,1> &__buffer);

#else

#define FORCE_IMPL_DATA_SRC_FCN(returns,fcn_name) \
  FORCE_IMPL_FCN(returns,fcn_name,__data_src<0> &__buffer); \
  FORCE_IMPL_FCN(returns,fcn_name,__data_src<1> &__buffer);

#define FORCE_IMPL_DATA_SRC_FCN_ARG(returns,fcn_name,...) \
  FORCE_IMPL_FCN(returns,fcn_name,__data_src<0> &__buffer,__VA_ARGS__); \
  FORCE_IMPL_FCN(returns,fcn_name,__data_src<1> &__buffer,__VA_ARGS__);

#define FORCE_IMPL_DATA_SRC_FCN_HDR(returns,fcn_name) \
  FORCE_IMPL_FCN(returns,fcn_name,subevent_header *__header,__data_src<0> &__buffer); \
  FORCE_IMPL_FCN(returns,fcn_name,subevent_header *__header,__data_src<1> &__buffer);

#endif

#endif//__DATA_SRC_FORCE_IMPL_H__
