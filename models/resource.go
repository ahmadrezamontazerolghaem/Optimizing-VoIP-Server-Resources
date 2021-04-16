type Resource struct {
	Id   string
	Name string

	Reason string
	Status string
	Type   string

	Time time.Time
}
type Resources map[string]Resource 

func ListResources(app string) (Resources, error) {
	res, err := CloudFormation().DescribeStackResources(&cloudformation.DescribeStackResourcesInput{
		StackName: aws.String(app),
	})

	if err != nil {
		return nil, err
	}

	resources := make(Resources, len(res.StackResources))

	for _, r := range res.StackResources {
		resources[*r.LogicalResourceId] = Resource{
			Id:     cs(r.PhysicalResourceId, ""),
			Name:   cs(r.LogicalResourceId, ""),
			Reason: cs(r.ResourceStatusReason, ""),
			Status: cs(r.ResourceStatus, ""),
			Type:   cs(r.ResourceType, ""),
			Time:   ct(r.Timestamp),
		}
	}

	return resources, nil
}
