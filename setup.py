from skbuild import setup

# Locations of packages providing configuration and data inputs
input_packages = {
    "CombineHarvester.CombineTools.input": "CombineTools/input",
    "CombineHarvester.CombineTools.input.examples": "CombineTools/input/examples",
    "CombineHarvester.CombineTools.input.job_prefixes": "CombineTools/input/job_prefixes",
    "CombineHarvester.CombineTools.input.xsecs_brs": "CombineTools/input/xsecs_brs",
}

base_packages = [
    "CombineHarvester",
    "CombineHarvester.CombineTools",
    "CombineHarvester.CombineTools.combine",
    "CombineHarvester.CombineTools.systematics",
    "CombineHarvester.CombineTools.scripts",
]

packages = base_packages + list(input_packages)

package_dir = {
    "CombineHarvester": "CombineHarvester",
    "CombineHarvester.CombineTools": "CombineTools/python",
    "CombineHarvester.CombineTools.combine": "CombineTools/python/combine",
    "CombineHarvester.CombineTools.systematics": "CombineTools/python/systematics",
    "CombineHarvester.CombineTools.scripts": "CombineTools/scripts",
    **input_packages,
}

package_data = {
    "CombineHarvester.CombineTools": ["*.so"],
    "CombineHarvester.CombineTools.scripts": ["*"],
    **{pkg: ["*"] for pkg in input_packages},
}

setup(
    name="combineharvester",
    version="0.1.0",
    description="CMS CombineHarvester tools",
    packages=packages,
    package_dir=package_dir,
    package_data=package_data,
    include_package_data=True,
)

