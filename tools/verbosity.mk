# Build system's output functionality
# -----------------------------------
VERB ?= full

OUTPUT =                                          \
	case "$(VERB)" in                         \
		full)                             \
			echo "  $1";              \
			;;                        \
		brief)                            \
			echo -n "  $2";           \
			if [ "$$?" != "0" ]; then \
				echo "FAILED";    \
			else                      \
				echo "OK";        \
			fi                        \
			;;                        \
		vim)                              \
			echo "  $2";              \
			;;                        \
	esac

