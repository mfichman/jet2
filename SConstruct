
import pkgboot

class Jet2(pkgboot.Package):
    defines = {}
    includes = []
    libs = [
        'glew32s.lib',
        'opengl32',
        'sfml-main.lib',
        'sfml-audio.lib',
        'sfml-graphics.lib',
        'sfml-network.lib',
        'sfml-system.lib',
        'sfml-window.lib',
        'BulletCollision.lib',
        'BulletDynamics.lib',
    ]
    major_version = '0'
    minor_version = '0'
    patch = '0'

    def build(self):
        self.env.VariantDir('build/code', 'code', duplicate=0)
        for code in self.env.Glob('build/code/**.cpp'):
            self.env.Depends(code, self.pch)
            name = code.name.replace('.cpp', '')
            lib = self.env.SharedLibrary('lib/%s' % name, (code, self.lib))


Jet2()
