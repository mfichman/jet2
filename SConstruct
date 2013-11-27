
import pkgboot

class Jet2(pkgboot.Package):
    defines = {}
    includes = [
        '/usr/local/include/bullet',
    ]
    libs = [
        pkgboot.Lib('glew32s', 'win32'),
        pkgboot.Lib('opengl32', 'win32'),
        pkgboot.Lib('ws2_32', 'win32'),
        'sfml-audio',
        'sfml-graphics',
        'sfml-network',
        'sfml-system',
        'sfml-window',
        'sfr',
        'coro',
        'BulletMultiThreaded',
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
