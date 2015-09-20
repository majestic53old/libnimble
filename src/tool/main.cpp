/**
 * libnimble
 * Copyright (C) 2015 David Jolly
 * ----------------------
 *
 * libnimble is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libnimble is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../lib/include/nimble.h"

int 
main(
	__in int argc,
	__in const char **argv,
	__in const char **envp
	)
{
	int result = 0;
	nimble_ptr inst = NULL;
	std::string path = std::string(argv[0]);

	TRACE_START_PATH(path.substr(0, path.find_last_of("/\\")));
	TRACE_ENTRY(TRACE_VERBOSE);

	try {
		inst = nimble::acquire();
		inst->initialize();
		result = inst->run(argc, argv, envp);
		inst->uninitialize();
	} catch(nimble_exception &exc) {
		TRACE_MESSAGE(TRACE_ERROR, "%s", CHK_STR(exc.to_string(true)));
		std::cerr << exc.to_string(true) << std::endl;
		result = INVALID_TYPE(int);
	} catch(std::exception &exc) {
		TRACE_MESSAGE(TRACE_ERROR, "%s", exc.what());
		std::cerr << exc.what() << std::endl;
		result = INVALID_TYPE(int);
	}
	
	TRACE_EXIT_MESSAGE(TRACE_VERBOSE, "res. 0x%x", result);
	TRACE_STOP();

	return result;
}
