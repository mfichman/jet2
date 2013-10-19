import platform
import os
import sys
import shutil
import subprocess
import shlex

(system, _, release, version, machine, proc) = platform.uname()
print(' '.join((system, release, version, machine, proc)))

VariantDir('build/src', 'src', duplicate=0)
VariantDir('build/test', 'test', duplicate=0)
VariantDir('build/code', 'code', duplicate=0)

env = Environment(CPPPATH=['build/src'])
env.Append(ENV=os.environ)

build_mode = ARGUMENTS.get('mode', 'debug')
major_version = '0'
minor_version = '1'
patch = '0'
version = '.'.join((major_version, minor_version, patch))
branch = os.popen('git rev-parse --abbrev-ref HEAD').read().strip()
revision = os.popen('git rev-parse HEAD').read().strip()
defines = {
    'VERSION': version,
    'REVISION': revision,
    'BRANCH': branch,
}
includes = ( 
    'src',
)

env.Append(CPPDEFINES=defines)
env.Append(CPPPATH=includes)

def test(target, source, env):
# Runs a single unit test and checks that the return code is 0
    try:
        subprocess.check_call(source[0].abspath)
    except subprocess.CalledProcessError, e:
        return 1

if env['PLATFORM'] == 'win32':
    env.Append(CXXFLAGS = '/MT /EHsc /Zi /Gm')
    env.Append(CXXFLAGS = '/Fpbuild/src/Common.pch')
    pchenv = env.Clone()
    pchenv.Append(CXXFLAGS = '/YcCommon.hpp')
    pch = pchenv.StaticObject('build/src/Common', 'build/src/Common.cpp')
    env.Append(CXXFLAGS = '/YuCommon.hpp')

src = filter(lambda x: 'Common.cpp' not in x.name, env.Glob('build/src/**.cpp'))
env.Depends(src, pch) # Wait for pch to build
jet2 = env.StaticLibrary('lib/jet2', (src, pch))

for code in env.Glob('build/code/**.cpp'):
    env.Depends(code, pch)
    name = code.name.replace('.cpp', '')
    lib = env.SharedLibrary('lib/%s' % name, (code, jet2))

if 'check' in COMMAND_LINE_TARGETS:
    env.Append(BUILDERS={'Test': Builder(action=test)})
    tests = []
    for test in env.Glob('build/test/**.cpp'):
        name = test.name.replace('.cpp', '')
        prog = env.Program('bin/test/%s' % name, (test, jet2))
        tests.append(env.Test(name, prog))
    env.Alias('check', tests)

