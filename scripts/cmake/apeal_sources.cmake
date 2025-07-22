# Automatically Generated *** DO NOT EDIT
# Generated Date: 2024-11-20

# Source Group: Apeal/src
set(src
  ${CONFIG_APEAL_SRC_DIR}/apeal_base58.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_cipher_cast128.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_console.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_debug.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_error.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_fs.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_image.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_image_bmp.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_image_convert.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_image_file.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_image_ico.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_image_libjpeg.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_image_libpng.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_image_libtiff.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_image_openjpeg.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_mutex.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_printer.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_process.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_random.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_thread.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_time.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_uid.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_uuid.cpp
  ${CONFIG_APEAL_SRC_DIR}/apeal_zlib.cpp
)
source_group(TREE ${CONFIG_APEAL_SRC_DIR} PREFIX "Sources\\Apeal/src" FILES ${src})
list(APPEND main_SRC ${src})

# Source Group: Apeal/hpp
set(src
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/arithm.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/ascii.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/assert.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/base58.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/buffer/buffer.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/buffer/buffer_base.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/buffer/fixed_buffer.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/buffer/linked_ring_buffer.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/buffer/pool_buffer.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/buffer/ring_buffer.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/buffer.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/char.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/cipher/cast128.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/cmdline.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/concepts.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/console.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/containers.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/cprep.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/crc/adler.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/crc/autodin.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/crc/crc.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/crc/crc64.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/crc/crc8.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/data_param.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/debug/debug_print_core.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/debug/debug_trap.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/debug.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/def/def_debug.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/def/def_stdlib.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/def/def_target_os.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/def.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/deprecated.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/empty.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/endian.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/enum_bitmask.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/err/eerrno.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/err/esock.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/err/eunix.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/err/ewin.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/error.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/event.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/file/apeal_file.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/file/fdir.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/file/filename.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/file/fs.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/file/fvolume.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/file/fwrtfmt.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/file.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_append.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_append_body.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_append_body_arg.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_arg.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_components.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_remainder.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_type.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_type_bool.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_type_char.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_type_geo.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_type_int.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_type_real.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_type_string.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_type_util.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_type_vector.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/format/str_format_util.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/fs.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/geo/margins.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/geo/point.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/geo/point3d.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/geo/rect.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/geo/size.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/geo.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/hash/md5.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/hash/sha256.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/hash.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/have.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/http.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/ignore.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/image/bmp.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/image/ico.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/image/libjpeg.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/image/libpng.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/image/libtiff.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/image/openjpeg.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/image.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/index.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/iterator.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/lang.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/logger.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/macos/cfobject.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/macos/nslog.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/macos/nsstring.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/meta/foreach.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/meta/iterator.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/meta/match.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/meta/search.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/meta/typelist.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/meta/typemap.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/mm/tinymem.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/mutex.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/noncopyable.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/on_exit.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/openssl.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/bool.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/csv.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/float.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/geo.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/http.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/ini.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/int.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/json.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/json_builder.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/json_object.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/json_parser.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/json_processor.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/number.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/parse_base.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/parse_file.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/parse_input.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/parse_stream.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/parse_util.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/parse_value.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/preprocessor.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/selection.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/split.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/string.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/uri.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/url.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/val_range.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/vector.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/xml.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/yaml.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/yaml_builder.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/yaml_generator.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/yaml_object.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/yaml_parser.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse/yaml_processor.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/parse.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/platform/android.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/platform/cygwin.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/platform/ios.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/platform/macos.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/platform/qt.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/platform/windows.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/policy/policy_exception.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/policy.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/printer.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/process.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/random.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/signal.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/singleton.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/socket.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/stl.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/str/case.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/str/custom.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/str/escape.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/str/format.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/str/join.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/str/replace.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/str/strview.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/str/sysapi.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/str/trim.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/stream.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/string.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/strview.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/tar.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/thread.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/time.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/typemap.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/types.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/uid.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/util.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/uuid.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/val_range.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/vector.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/verify.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/void.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/windows/com.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/windows/disk_volume.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/windows/ole.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/windows/ole_bstr.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/windows/ole_idispatch.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/windows/ole_iunknown.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/windows/ole_variant.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/windows/registry.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/windows/windows.hpp
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/xaos/xsapi.h
  ${CONFIG_APEAL_INCLUDE_DIR}/apeal/zlib.hpp
)
source_group(TREE ${CONFIG_APEAL_INCLUDE_DIR} PREFIX "Sources\\Apeal/hpp" FILES ${src})
list(APPEND main_SRC ${src})

# Source Group: Apeal/3rdparty/base58
set(src
  ${CONFIG_APEAL_3RDPARTY_DIR}/base58/base58.cpp
  ${CONFIG_APEAL_3RDPARTY_DIR}/base58/base58.h
)
source_group(TREE ${CONFIG_APEAL_3RDPARTY_DIR}/base58 PREFIX "Sources\\Apeal/3rdparty/base58" FILES ${src})
list(APPEND main_SRC ${src})

# Source Group: Apeal/3rdparty/zlib
if (NOT IOS)
  add_compile_definitions(HAVE_ZLIB=1)
  set(src
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_adler32.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_compress.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_crc32.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_crc32.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_deflate.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_deflate.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_gzclose.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_gzguts.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_gzlib.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_gzread.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_gzwrite.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_infback.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_inffast.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_inffast.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_inffixed.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_inflate.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_inflate.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_inftrees.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_inftrees.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_trees.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_trees.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_uncompr.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_zconf.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_zlib.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_zutil.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/zlib/zlib_zutil.h
  )
  source_group(TREE ${CONFIG_APEAL_3RDPARTY_DIR}/zlib PREFIX "Sources\\Apeal/3rdparty/zlib" FILES ${src})
  list(APPEND main_SRC ${src})
  
if (MSVC)
    set_source_files_properties(${src} PROPERTIES COMPILE_FLAGS "/W0")
else()
    set_source_files_properties(${src} PROPERTIES COMPILE_FLAGS "-w")
endif()

  set_source_files_properties(${src} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
endif()

# Source Group: Apeal/3rdparty/libpng
if (NOT IOS)
  add_compile_definitions(HAVE_LIBPNG=1)
  include_directories(${CONFIG_APEAL_3RDPARTY_DIR}/libpng)
  set(src
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/arm/arm_init.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/arm/filter_neon_intrinsics.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/arm/palette_neon_intrinsics.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/intel/filter_sse2_intrinsics.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/intel/intel_init.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/loongarch/filter_lsx_intrinsics.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/loongarch/loongarch_lsx_init.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/mips/filter_mmi_inline_assembly.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/mips/filter_msa_intrinsics.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/mips/mips_init.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/png.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/png.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngconf.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngdebug.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngerror.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngget.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pnginfo.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pnglibconf.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngmem.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngpread.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngpriv.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngread.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngrio.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngrtran.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngrutil.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngset.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngstruct.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngtrans.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngwio.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngwrite.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngwtran.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/pngwutil.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/powerpc/filter_vsx_intrinsics.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/libpng/powerpc/powerpc_init.c
  )
  source_group(TREE ${CONFIG_APEAL_3RDPARTY_DIR}/libpng PREFIX "Sources\\Apeal/3rdparty/libpng" FILES ${src})
  list(APPEND main_SRC ${src})
  
if (MSVC)
    set_source_files_properties(${src} PROPERTIES COMPILE_FLAGS "/W0")
else()
    set_source_files_properties(${src} PROPERTIES COMPILE_FLAGS "-w")
endif()

  set_source_files_properties(${src} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
endif()

# Source Group: Apeal/3rdparty/uuid
if (NOT WIN32 AND NOT IOS)
  set(src
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_clear.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_compare.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_config.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_copy.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_gen_uuid.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_isnull.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_pack.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_parse.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_time.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_types.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_unpack.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_unparse.c
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_uuid.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_uuidd.h
    ${CONFIG_APEAL_3RDPARTY_DIR}/uuid/uuid_uuidP.h
  )
  source_group(TREE ${CONFIG_APEAL_3RDPARTY_DIR}/uuid PREFIX "Sources\\Apeal/3rdparty/uuid" FILES ${src})
  list(APPEND main_SRC ${src})
  
if (MSVC)
    set_source_files_properties(${src} PROPERTIES COMPILE_FLAGS "/W0")
else()
    set_source_files_properties(${src} PROPERTIES COMPILE_FLAGS "-w")
endif()

  set_source_files_properties(${src} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
endif()

