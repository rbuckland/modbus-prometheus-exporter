IN_NESTED_DOCKER =


#
# This target re-runs the same Makefile and goals passed at the command line
# but inside the docker image
#
docker-nested-run:
    # extract the image name, above the intended target
	$(eval _DOCKER_IMAGE_ := $(shell awk '/^$(MAKECMDGOALS):/{if (a[0] && a[0] != /$(MAKECMDGOALS):/) print a[1];} { a[0]=$$0; a[1]=$$2 }' $(realpath $(firstword $(MAKEFILE_LIST))) ))
ifneq ($(IN_NESTED_DOCKER),yes)
	@echo "❖ re-launching '$(MAKECMDGOALS)' in $(_DOCKER_IMAGE_)"
	$(eval PROJECT_DIR=$(shell basename $(PWD)))
	docker run --rm -ti --user $$(id -u):$$(id -g) -e IN_NESTED_DOCKER=yes $(DOCKER_OPTS) -v "$(PWD)":/$(PROJECT_DIR) --workdir /$(PROJECT_DIR) $(_DOCKER_IMAGE_) make IN_NESTED_DOCKER=yes -f $(firstword $(MAKEFILE_LIST)) $(MAKECMDGOALS);
	#
	#  if we wanted to avoid "if guards" guards around the make lines, we could workout how to "stop" Make right here.
	# kill -9 $(shell ps -o ppid $$$$ | tail -n +2 | head -1) 
endif


