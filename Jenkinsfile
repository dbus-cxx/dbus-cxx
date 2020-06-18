pipeline {
	agent { label 'master' }

	stages {
		stage('Cleanup'){
			steps{
				cleanWs()
			}
		}

		stage('Checkout'){
			steps{
				checkout scm
			}
		}

		stage('Build'){
			buildDebPkg( "amd64", "buster" )
		}
	}
}

void buildDebPkg(String arch, String distro){
	debianPbuilder additionalBuildResults: '', 
			architecture: arch, 
			components: '', 
			distribution: distro, 
			keyring: '', 
			mirrorSite: 'https://deb.debian.net/debian', 
			pristineTarName: ''
}
