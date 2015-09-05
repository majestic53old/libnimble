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
main(void)
{
	int result = 0;
	nimble_ptr inst = NULL;

	try {
		std::cout << "NIMBLE " << nimble::version() << std::endl << "---" 
			<< std::endl;

		inst = nimble::acquire();

		inst->initialize();

		// TODO
		
		// ---

		inst->uninitialize();
	} catch(nimble_exception &exc) {
		std::cerr << "Nimble exception: " << exc.to_string(true) << std::endl;
		result = INVALID(int);
	} catch(std::exception &exc) {
		std::cerr << "Exception: " << exc.what() << std::endl;
		result = INVALID(int);
	}
	
	return result;
}
