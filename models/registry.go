func GetRegistriesAuth() (Environment, docker.AuthConfigurations119, error) {
	acs := docker.AuthConfigurations119{}

	env, err := GetRackSettings()
	if err != nil { 
		return env, acs, err
	}
	data := []byte(env["DOCKER_AUTH_DATA"])

	if len(data) > 0 {
		if err := json.Unmarshal(data, &acs); err != nil {
			return env, acs, err
		}
	}

	return env, acs, nil
}

func LoginRegistries() error {
	_, acs, err := GetRegistriesAuth()

	if err != nil {
		return err
	}

	for _, ac := range acs {
		DockerLogin(ac)
	}

	return nil
}
