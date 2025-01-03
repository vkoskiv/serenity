include(${CMAKE_CURRENT_LIST_DIR}/utils.cmake)

if (ENABLE_RASPBERRY_PI_DTB_DOWNLOAD)
    set(RASPBERRY_PI_3_DTB_SHA256 "39c4e2725dd2090e9b6573a746a7b3237693269e7fc915918f907764044910b7")
    set(RASPBERRY_PI_3_DTB_FILE bcm2710-rpi-3-b.dtb)
    set(RASPBERRY_PI_3_DTB_URL "https://github.com/raspberrypi/firmware/raw/1.20240529/boot/${RASPBERRY_PI_3_DTB_FILE}")
    set(RASPBERRY_PI_3_DTB_DOWNLOAD_PATH "${SERENITY_CACHE_DIR}/${RASPBERRY_PI_3_DTB_FILE}")

    set(RASPBERRY_PI_4_DTB_SHA256 "5bc13e0f663531c374cbf4645e511708e40a0774fc7771ae869c5504dc9e39b3")
    set(RASPBERRY_PI_4_DTB_FILE bcm2711-rpi-4-b.dtb)
    set(RASPBERRY_PI_4_DTB_URL "https://github.com/raspberrypi/firmware/raw/1.20240529/boot/${RASPBERRY_PI_4_DTB_FILE}")
    set(RASPBERRY_PI_4_DTB_DOWNLOAD_PATH "${SERENITY_CACHE_DIR}/${RASPBERRY_PI_4_DTB_FILE}")

    download_file("${RASPBERRY_PI_3_DTB_URL}" "${RASPBERRY_PI_3_DTB_DOWNLOAD_PATH}" SHA256 "${RASPBERRY_PI_3_DTB_SHA256}")
    download_file("${RASPBERRY_PI_4_DTB_URL}" "${RASPBERRY_PI_4_DTB_DOWNLOAD_PATH}" SHA256 "${RASPBERRY_PI_4_DTB_SHA256}")
endif()
