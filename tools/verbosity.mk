# Build system's output functionality
# -----------------------------------
VERB ?= full

ifeq ($(VERB),brief)
  FAILURE = echo "FAILED"
  SUCCESS = echo "OK"
else
  FAILURE = true
  SUCCESS = true
endif

EXEC =                                            \
	$1;                                       \
	if [ "$$?" != "0" ]; then                 \
		$(FAILURE);                       \
		false;                            \
	else                                      \
		$(SUCCESS);                       \
	fi

OUTPUT =                                          \
	case "$(VERB)" in                         \
		full)                             \
			echo "  $1";              \
			$(call EXEC,$1);          \
			;;                        \
		brief)                            \
			echo -n "  $2";           \
			$(call EXEC,$1);          \
			;;                        \
		vim)                              \
			echo "  $2";              \
			$(call EXEC,$1);          \
			;;                        \
	esac

