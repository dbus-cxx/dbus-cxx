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
				fileOperations([folderCreateOperation('source')])
				dir('source'){
					checkout scm
				}
			}
		}

		stage('Build'){
			steps{
				configFileProvider([configFile(fileId: '42dd2363-51ed-4972-a382-f25ddbe11b3a', targetLocation: 'hookdir/D21-nightly-dbuscxx')]) {
					buildDebPkg( "amd64", "buster" )
				}
			}
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
