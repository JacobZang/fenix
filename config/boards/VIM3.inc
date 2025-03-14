#!/bin/bash
###################################
## VIM3 series common part
###################################

#### Packing image
IMAGE_PACKAGE_CONF=package_g12.conf
IMAGE_PLATFORM_CONF=platform_g12.conf

###################################
## Include VIM series common part
###################################
source config/boards/VIM.inc

###################################
## tweaks for platform
tweaks_platform() {
	echo "#Serial console allow root" >> $ROOTFS_TEMP/etc/securetty
	echo "ttyAML0" >> $ROOTFS_TEMP/etc/securetty
	# Setup GST environment
	echo "export GST_DEFAULT_V4L2_BUF_MODE=DMA_BUF_IMPORT" >> $ROOTFS_TEMP/etc/bash.bashrc

	# Enable services
	execute_in_chroot "systemctl --no-reload enable fan.service"

	if [ "$LINUX" != "mainline" ]; then
		execute_in_chroot "systemctl --no-reload enable panel.service"
		if [ "$DISTRIB_TYPE" != "server" ]; then
			execute_in_chroot "systemctl --no-reload enable tone2-control.service"
		fi
	fi

	if [ "$LINUX" != "mainline" ]; then
		execute_in_chroot "systemctl --no-reload enable bluetooth-khadas.service"
	fi

	# Create boot links
	if [ "$INSTALL_TYPE" == "EMMC" ]; then
		if [ "$LINUX" != "5.15" ]; then
			execute_in_chroot "ln -fs /boot/zImage zImage"
			execute_in_chroot "ln -fs /boot/uInitrd uInitrd"
			execute_in_chroot "ln -fs /boot/dtb/$(basename $LINUX_DTB) dtb.img"
		else
			execute_in_chroot "ln -fs /boot/Image Image"
			execute_in_chroot "ln -fs /boot/initrd.img initrd.img"
			execute_in_chroot "ln -fs /boot/dtb/amlogic/$(basename $LINUX_DTB) dtb.img"
			execute_in_chroot "ln -fs /boot/dtb dtb"
		fi
	else
		# Setup default DTB for SD/USB images
		if [ "$LINUX" == "mainline" ] || [ "$LINUX" == "5.15" ]; then
			execute_in_chroot "ln -fs dtb/amlogic/$(basename $LINUX_DTB) /boot/dtb.img || cp /boot/dtb/amlogic/$(basename $LINUX_DTB) /boot/dtb.img"
		else
			execute_in_chroot "cp /boot/dtb/$(basename $LINUX_DTB) /boot/dtb.img"
		fi
	fi

	if [ "$LINUX" != "mainline" ] && [ "$PANFROST_SUPPORT" == "yes" ]; then
		local overlay_env="$ROOTFS_TEMP/boot/dtb/${VENDOR,,}/$(basename $LINUX_DTB).overlay.env"
		sed -i --follow-symlinks '/fdt_overlays/s/=/=panfrost /g' $overlay_env
	fi
}

## Build deb packages for platform
build_deb_packages_platform() {

	if  [ "$DISTRIB_RELEASE" == "noble" ] ; then
		info_msg "Building dracut_install package..."
		# FIXME
		# remove build stamp to force build for other arch
		rm -rf $BUILD/.stamps/dracut_install
		build_package "dracut_install:target"
	fi

	if [ "$DISTRIB_TYPE" == "minimal" ]; then
		warning_msg "Ignore build platform packages for minimal image."
		return 0
	fi

	if [ "$LINUX" == "5.15" ]; then

		if [ "$DISTRIB_RELEASE" == "jammy" ] || [ "$DISTRIB_RELEASE" == "noble" ]; then
			info_msg "Building npu_aml packages..."
			# FIXME
			# remove build stamp to force build for other arch
			rm -rf $BUILD/.stamps/npu_aml
			build_package "npu_aml:target"
		fi

		if ([ "$DISTRIB_RELEASE" == "jammy" ] || [ "$DISTRIB_RELEASE" == "bookworm" ] || [ "$DISTRIB_RELEASE" == "noble" ]) && [ "$DISTRIB_TYPE" != "server" ]; then
			if [ "$PANFROST_SUPPORT" != "yes" ]; then
				info_msg "Building mali-debs package..."
				# FIXME
				# remove build stamp to force build for other arch
				rm -rf $BUILD/.stamps/mali-debs
				build_package "mali-debs:target"
			fi
		fi

		if  [ "$DISTRIB_RELEASE" == "noble" ] && [ "$DISTRIB_TYPE" != "server" ]; then
			if [ "$PANFROST_SUPPORT" != "yes" ]; then
				info_msg "Building wayland-debs package..."
				# FIXME
				# remove build stamp to force build for other arch
				rm -rf $BUILD/.stamps/wayland-debs
				build_package "wayland-debs:target"
			fi

			info_msg "Building chromium-debs package..."
			# FIXME
			# remove build stamp to force build for other arch
			rm -rf $BUILD/.stamps/chromium-debs
			build_package "chromium-debs:target"
		fi

		info_msg "Building OPTEE userspace package..."
		# FIXME
		# remove build stamp to force build for other arch
		rm -rf $BUILD/.stamps/optee_userspace_deb_aml
		build_package "optee_userspace_deb_aml:target"

		info_msg "Building OPTEE video firmware package..."
		# FIXME
		# remove build stamp to force build for other arch
		rm -rf $BUILD/.stamps/optee_video_firmware_deb_aml
		build_package "optee_video_firmware_deb_aml:target"

		info_msg "Building libion package..."
		# FIXME
		# remove build stamp to force build for other arch
		rm -rf $BUILD/.stamps/libion_deb_aml
		build_package "libion_deb_aml:target"

		info_msg "Building libge2d package..."
		# FIXME
		# remove build stamp to force build for other arch
		rm -rf $BUILD/.stamps/libge2d_deb_aml
		build_package "libge2d_deb_aml:target"

		info_msg "Building libjpegenc package..."
		# FIXME
		# remove build stamp to force build for other arch
		rm -rf $BUILD/.stamps/libjpegenc_deb_aml
		build_package "libjpegenc_deb_aml:target"

		# FIXME
		# remove build stamp to force build for other arch
		rm -rf $BUILD/.stamps/libamvenc_deb_aml
		build_package "libamvenc_deb_aml:target"

		info_msg "Building multimedia package..."
		# FIXME
		# remove build stamp to force build for other arch
		rm -rf $BUILD/.stamps/multimedia_debs_aml
		build_package "multimedia_debs_aml:target"

		if [ "$DISTRIB_RELEASE" == "jammy" ] || [ "$DISTRIB_RELEASE" == "noble" ]; then
			info_msg "Building gstreamer_aml package..."
			# FIXME
			# remove build stamp to force build for other arch
			rm -rf $BUILD/.stamps/gstreamer_aml
			build_package "gstreamer_aml:target"
		fi

		info_msg "Building meson-display package..."
		# FIXME
		# remove build stamp to force build for other arch
		rm -rf $BUILD/.stamps/meson-display-deb-aml
		build_package "meson-display-deb-aml:target"
	fi
}

## Install deb packages for platform
install_deb_packages_platform() {

	if [ "$DISTRIB_RELEASE" == "noble" ]; then
		info_msg "Installing dracut_install package ..."
		install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/$DISTRIBUTION-$DISTRIB_RELEASE/dracut_install/*.deb
	fi

	if [ "$DISTRIB_TYPE" == "minimal" ]; then
		warning_msg "Ignore install platform packages for minimal image."
		return 0
	fi

	if [ "$LINUX" == "5.15" ]; then

		cd $ROOT

		if ([ "$DISTRIB_RELEASE" == "noble" ]) && [ "$DISTRIB_TYPE" != "server" ]; then
			if [ "$PANFROST_SUPPORT" != "yes" ]; then
				info_msg "Installing wayland-debs package ..."
				install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/wayland-debs/libwayland-client0_*.deb
				install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/wayland-debs/libwayland-server0_*.deb
				install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/wayland-debs/libwayland-cursor0_*.deb
				install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/wayland-debs/libwayland-egl1_*.deb
			fi

			info_msg "Installing chromium-debs package ..."
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/chromium-debs/chromium-common_*.deb
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/chromium-debs/chromium_*.deb
		fi

		if [ "$DISTRIB_RELEASE" == "jammy" ] || [ "$DISTRIB_RELEASE" == "noble" ]; then
			info_msg "Installing npu_aml packages..."
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/npu_aml/*.deb
		fi

		if ([ "$DISTRIB_RELEASE" == "jammy" ] || [ "$DISTRIB_RELEASE" == "noble" ] || [ "$DISTRIB_RELEASE" == "bookworm" ]) && [ "$DISTRIB_TYPE" != "server" ]; then
			if [ "$PANFROST_SUPPORT" != "yes" ]; then
				info_msg "Installing mali-debs package ..."
				install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/$DISTRIBUTION-$DISTRIB_RELEASE/mali-debs/${GPU_PLATFORM}/*.deb
			fi
		fi

		info_msg "Installing OPTEE userspace package..."
		install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/optee_userspace_deb_aml/*.deb

		info_msg "Installing OPTEE video firmware package..."
		install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/optee_video_firmware_deb_aml/*.deb

		info_msg "Installing libion package..."
		install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/libion_deb_aml/*.deb

		info_msg "Installing libge2d package..."
		install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/libge2d_deb_aml/*.deb

		info_msg "Installing libjpegenc package..."
		install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/libjpegenc_deb_aml/*.deb

		info_msg "Installing libamvenc package..."
		install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/libamvenc_deb_aml/*.deb

		# Multimdia
		info_msg "Installing multimdia package..."
		install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/multimedia_debs_aml/gst-aml-drmbufferpool-plugins_*.deb
		install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/multimedia_debs_aml/mediactrlsrc_*.deb
		install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/multimedia_debs_aml/ispaml_*.deb

		if [ "$DISTRIB_RELEASE" == "jammy" ] || [ "$DISTRIB_RELEASE" == "noble" ]; then
			info_msg "Installing gstreamer_aml package..."
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gstreamer-aml_*.deb
			# gstreamer1.0
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gstreamer1.0-debs/libgstreamer1.0-0_*.deb
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gstreamer1.0-debs/gir1.2-gstreamer-1.0_*.deb
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gstreamer1.0-debs/gstreamer1.0-tools_*.deb
			# gst-plugins-base1.0
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gst-plugins-base1.0-debs/libgstreamer-plugins-base1.0-0_*.deb
			#install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gst-plugins-base1.0-debs/libgstreamer-gl1.0-0_*.deb
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gst-plugins-base1.0-debs/gstreamer1.0-plugins-base_*.deb
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gst-plugins-base1.0-debs/gstreamer1.0-plugins-base-apps_*.deb
			#install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gst-plugins-base1.0-debs/gstreamer1.0-x_*.deb
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gst-plugins-base1.0-debs/gstreamer1.0-alsa_*.deb
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gst-plugins-base1.0-debs/gir1.2-gst-plugins-base-1.0_*.deb
			# gst-plugins-good1.0
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gst-plugins-good1.0-debs/libgstreamer-plugins-good1.0-0_*.deb
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gst-plugins-good1.0-debs/gstreamer1.0-plugins-good_*.deb
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gst-plugins-good1.0-debs/gstreamer1.0-pulseaudio_*.deb
			# gst-plugins-bad1.0
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gst-plugins-bad1.0-debs/libgstreamer-plugins-bad1.0-0_*.deb
			install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/gstreamer_aml/gst-plugins-bad1.0-debs/gstreamer1.0-plugins-bad_*.deb
		fi

		info_msg "Installing meson-display package..."
		install_deb_chroot $BUILD_DEBS/$VERSION/$KHADAS_BOARD/${DISTRIBUTION}-${DISTRIB_RELEASE}/meson-display-deb-aml/*.deb
	fi
}
