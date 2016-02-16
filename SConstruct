
import pkgboot
import os

class Jet2(pkgboot.Package):
    defines = {}
    includes = [
        os.path.join(os.environ.get('LOCALAPPDATA', ''), 'WinBrew\\include\\freetype'),
        '/usr/local/include/bullet',
        '/usr/local/include/freetype2',
        '../coro/include',
    ]
    libs = [
        pkgboot.Lib('glew', 'win32'),
        pkgboot.Lib('opengl32', 'win32'),
        pkgboot.Lib('ws2_32', 'win32'),
        pkgboot.Lib('user32', 'win32'),
        'sfml-audio',
        'sfml-graphics',
        'sfml-network',
        'sfml-system',
        'sfml-window',
        'sfr',
        'coro',
        'freetype',
        'BulletSoftBody',
        'BulletDynamics',
        'BulletCollision',
        'LinearMath',
    ]
    frameworks = [
        'OpenGL',
    ]
    major_version = '0'
    minor_version = '0'
    patch = '0'

    def build(self):
        self.env.VariantDir('build/code', 'code', duplicate=0)
        for code in self.env.Glob('build/code/**.cpp'):
            self.env.Depends(code, self.pch)
            name = code.name.replace('.cpp', '').lower()
            lib = self.env.SharedLibrary('lib/%s' % name, (code, self.lib))

Jet2()
