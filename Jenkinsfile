pipeline {
agent {
    // Equivalent to "docker build -f Dockerfile.build --build-arg version=1.0.2 ./build/
    dockerfile {
        dir 'docker'
        
    }
}
	stages {
        stage('Build') {
            steps {
                cmake installation: 'InSearchPath'
                cmakeBuild buildType: 'Release', cleanBuild: true, installation: 'InSearchPath', steps: [[withCmake: true]]
            }
        }

        stage('Test') {
            steps {
                ctest 'InSearchPath'
            }
        }

}
}