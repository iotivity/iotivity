# ------------------------------------------------------------------------
# Copyright 2015 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ------------------------------------------------------------------------

from default_configuration import *
from gcc_configuration import *

# Canonicalize the C++ compiler name to "g++" if gcc is being used to
# simplify mapping to the appropriate C++11 flags since gcc may be
# installed under a different name.  This will be used when mapping
# compiler name to configuration in the factory submodule.
_GCC = 'g++'

# Update this dictionary with new compiler configurations as needed.
_CONFIG_MAP = { _GCC : GccConfiguration }

_compiler_config = None

def check_for_gcc_cxx(context):
    """
    Check if the C++ compiler is GCC

    Returns 1 if gcc, 0 otherwise
    """

    test_program = """
#if !defined(__GNUC__) || !defined(__cplusplus)
#  error "Not the GCC C++ compiler."
#endif

class foo
{
public:
    foo() : x_() {}
    int x() const { return x_; }
private:
    int x_;
};
"""

    return context.TryCompile(test_program, '.cpp')

def make_compiler_config(context):
    """
    Create compiler-specific configuration object.

    Arguments:
    context -- the scons configure context

    The 'CXX' key in the SCons environment will be mapped to the
    appropriate supported compiler configuration.  If no match is
    found compiler configuration operations will simply be no-ops.
    """

    global _compiler_config

    if _compiler_config is None:
        cxx = context.env['CXX']

        if check_for_gcc_cxx(context):
            cxx = _GCC

        config = _CONFIG_MAP.get(cxx, DefaultConfiguration)

        _compiler_config = config(context)

    return _compiler_config
