#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define KVM_PATH		"/dev/kvm"

#define KVMIO			0xAE
#define KVM_CHECK_EXTENTION	_IO(KVMIO, 0x03)

void usage(int argc, char **argv)
{
	printf("Usage: %s base_cap [num_caps]\n"
		"\tbase_cap - first cap to print\n"
		"\tnum_caps - number of caps to print starting from base_cap\n"
		"\n"
		"Print vals of kvm extentions base_cap -> base_cap + num_caps\n",
		argv[0]);
}

int main(int argc, char **argv)
{
	int kvm_fd;
	int base_cap, num_caps = 1;

	if (argc < 2 || argc > 3) {
		usage(argc, argv);
		return -1;
	}

	base_cap = strtol(argv[1], NULL, 10);
	if (errno) {
		int err = errno;
		printf("Error parsing base_cap (%d %s)\n", err, strerror(err));
		return -err;
	}

	if (argc == 3) {
		num_caps = strtol(argv[2], NULL, 10);
		if (errno) {
			printf("Error parsing num_caps (%d %s)\n",
			       errno, strerror(errno));
			num_caps = 1;
		} else if (num_caps <= 0) {
			num_caps = 1;
		}
	}

	kvm_fd = open(KVM_PATH, O_RDWR);
	if (kvm_fd < 0) {
		int err = errno;
		printf("Failed to open %s (%d %s)\n", KVM_PATH, err,
						      strerror(err));
		return -err;
	}

	for (; num_caps; ++base_cap, --num_caps) {
		int ret;

		ret = ioctl(kvm_fd, KVM_CHECK_EXTENTION, base_cap);
		if (ret < 0)
			ret = -errno;

		printf("Extension %d: %d %s\n", base_cap, ret,
				ret < 0 ? strerror(-ret) : "");
	}

	close(kvm_fd);
}
