cmake_minimum_required (VERSION 3.6)

project(UniqueBase CXX)

aux_source_directory(. DIR_SRCS)

aux_source_directory(./Container Container_DIR_SRCS)
aux_source_directory(./Core Core_DIR_SRCS)
aux_source_directory(./IO IO_DIR_SRCS)
aux_source_directory(./Math Math_DIR_SRCS)
aux_source_directory(./Resource Resource_DIR_SRCS)
aux_source_directory(./Serialize Serialize_DIR_SRCS)

source_group(Container FILES ${Container_DIR_SRCS})
source_group(Core FILES ${Core_DIR_SRCS})
source_group(IO FILES ${IO_DIR_SRCS})
source_group(Math FILES ${Math_DIR_SRCS})
source_group(Resource FILES ${Resource_DIR_SRCS})
source_group(Serialize FILES ${Serialize_DIR_SRCS})


add_library(UniqueBase STATIC 
${DIR_SRCS} 
${Container_DIR_SRCS}
${Core_DIR_SRCS}
${IO_DIR_SRCS}
${Math_DIR_SRCS}
${Resource_DIR_SRCS}
${Serialize_DIR_SRCS}

)

