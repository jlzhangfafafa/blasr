import com.atlassian.bamboo.specs.api.BambooSpec;
import com.atlassian.bamboo.specs.api.builders.AtlassianModule;
import com.atlassian.bamboo.specs.api.builders.BambooKey;
import com.atlassian.bamboo.specs.api.builders.BambooOid;
import com.atlassian.bamboo.specs.api.builders.plan.Job;
import com.atlassian.bamboo.specs.api.builders.plan.Plan;
import com.atlassian.bamboo.specs.api.builders.plan.PlanIdentifier;
import com.atlassian.bamboo.specs.api.builders.plan.Stage;
import com.atlassian.bamboo.specs.api.builders.plan.artifact.Artifact;
import com.atlassian.bamboo.specs.api.builders.plan.branches.BranchCleanup;
import com.atlassian.bamboo.specs.api.builders.plan.branches.PlanBranchManagement;
import com.atlassian.bamboo.specs.api.builders.project.Project;
import com.atlassian.bamboo.specs.api.builders.requirement.Requirement;
import com.atlassian.bamboo.specs.api.builders.task.AnyTask;
import com.atlassian.bamboo.specs.api.model.BambooKeyProperties;
import com.atlassian.bamboo.specs.api.model.BambooOidProperties;
import com.atlassian.bamboo.specs.api.model.plan.JobProperties;
import com.atlassian.bamboo.specs.api.model.plan.PlanIdentifierProperties;
import com.atlassian.bamboo.specs.api.model.plan.PlanProperties;
import com.atlassian.bamboo.specs.api.model.plan.StageProperties;
import com.atlassian.bamboo.specs.api.model.plan.artifact.ArtifactProperties;
import com.atlassian.bamboo.specs.api.model.plan.branches.PlanBranchManagementProperties;
import com.atlassian.bamboo.specs.api.model.plan.requirement.RequirementProperties;
import com.atlassian.bamboo.specs.api.model.project.ProjectProperties;
import com.atlassian.bamboo.specs.api.model.task.AnyTaskProperties;
import com.atlassian.bamboo.specs.builders.task.ArtifactDownloaderTask;
import com.atlassian.bamboo.specs.builders.task.CheckoutItem;
import com.atlassian.bamboo.specs.builders.task.DownloadItem;
import com.atlassian.bamboo.specs.builders.task.ScriptTask;
import com.atlassian.bamboo.specs.builders.task.TestParserTask;
import com.atlassian.bamboo.specs.builders.task.VcsCheckoutTask;
import com.atlassian.bamboo.specs.builders.trigger.BitbucketServerTrigger;
import com.atlassian.bamboo.specs.builders.trigger.RemoteTrigger;
import com.atlassian.bamboo.specs.builders.trigger.ScheduledTrigger;
import com.atlassian.bamboo.specs.model.task.ArtifactDownloaderTaskProperties;
import com.atlassian.bamboo.specs.model.task.DownloadItemProperties;
import com.atlassian.bamboo.specs.model.task.ScriptTaskProperties;
import com.atlassian.bamboo.specs.model.task.TestParserTaskProperties;
import com.atlassian.bamboo.specs.model.task.VcsCheckoutTaskProperties;
import com.atlassian.bamboo.specs.model.trigger.BitbucketServerTriggerProperties;
import com.atlassian.bamboo.specs.model.trigger.RemoteTriggerProperties;
import com.atlassian.bamboo.specs.model.trigger.ScheduledTriggerProperties;
import com.atlassian.bamboo.specs.util.BambooServer;
import com.atlassian.bamboo.specs.util.MapBuilder;

@BambooSpec
public class PlanSpec {
    public static void main(String... argv) {
        //By default credentials are read from the '.credentials' file.
        BambooServer bambooServer = new BambooServer("http://localhost:8085");
        
        Plan rootObject = new Plan(new Project()
                    .oid(new BambooOid("1jx1kzc2o5xc1"))
                    .key(new BambooKey("PLAYG")),
                "blasr-move bamboo inline build scripts to bitbucket",
                new BambooKey("BBIBSTB"))
                .oid(new BambooOid("1jwrvrqpgcf88"))
                .enabled(false)
                .stages(new Stage("sonarqube")
                        .jobs(new Job("SonarQube",
                                new BambooKey("SON"))
                                .tasks(new VcsCheckoutTask()
                                        .description("Checkout Default Repository")
                                        .checkoutItems(new CheckoutItem().defaultRepository()),
                                    new AnyTask(new AtlassianModule("ch.mibex.bamboo.sonar4bamboo:sonar4bamboo.sonarscannertask"))
                                        .configuration(new MapBuilder()
                                                .put("incrementalFileForInclusionList", "")
                                                .put("chosenSonarConfigId", "4")
                                                .put("useGradleWrapper", "")
                                                .put("useNewGradleSonarQubePlugin", "")
                                                .put("sonarJavaSource", "")
                                                .put("sonarProjectName", "SAT-blasr")
                                                .put("buildJdk", "JDK 1.8.0_101")
                                                .put("gradleWrapperLocation", "")
                                                .put("sonarLanguage", "")
                                                .put("sonarSources", "iblasr, ./Blasr.cpp")
                                                .put("useGlobalSonarServerConfig", "true")
                                                .put("incrementalMode", "")
                                                .put("failBuildForBrokenQualityGates", "")
                                                .put("sonarTests", "")
                                                .put("failBuildForSonarErrors", "")
                                                .put("sonarProjectVersion", "5.3.0.${bamboo.buildNumber}")
                                                .put("sonarBranch", "")
                                                .put("executable", "SonarScannerHome")
                                                .put("illegalBranchCharsReplacement", "_")
                                                .put("failBuildForTaskErrors", "true")
                                                .put("incrementalModeNotPossible", "incrementalModeRunFullAnalysis")
                                                .put("sonarJavaTarget", "")
                                                .put("environmentVariables", "")
                                                .put("incrementalModeGitBranchPattern", "")
                                                .put("legacyBranching", "")
                                                .put("replaceSpecialBranchChars", "")
                                                .put("additionalProperties", "-Dsonar.cfamily.build-wrapper-output.bypass=true")
                                                .put("autoBranch", "true")
                                                .put("sonarProjectKey", "SAT-blasr")
                                                .put("incrementalModeBambooUser", "")
                                                .put("overrideSonarBuildConfig", "true")
                                                .put("workingSubDirectory", "")
                                                .build()))
                                .requirements(new Requirement("system.os")
                                        .matchValue("linux")
                                        .matchType(Requirement.MatchType.EQUALS))),
                    new Stage("Default Stage")
                        .jobs(new Job("Building",
                                new BambooKey("JOB1"))
                                .artifacts(new Artifact()
                                        .name("prebuilt_blasr")
                                        .copyPattern("blasr-*.tgz")
                                        .location("tarballs"))
                                .tasks(new VcsCheckoutTask()
                                        .description("Checkout Default Repository")
                                        .checkoutItems(new CheckoutItem().defaultRepository())
                                        .cleanCheckout(true),
                                    new ArtifactDownloaderTask()
                                        .description("pbbam artifact")
                                        .enabled(false)
                                        .sourcePlan(new PlanIdentifier()
                                                .key(new BambooKey("PBBAM"))
                                                .projectKey(new BambooKey("SAT")))
                                        .artifacts(new DownloadItem()
                                                .allArtifacts(true)),
                                    new ArtifactDownloaderTask()
                                        .description("blasr_libcpp artifact")
                                        .enabled(false)
                                        .sourcePlan(new PlanIdentifier()
                                                .key(new BambooKey("BLC"))
                                                .projectKey(new BambooKey("SAT")))
                                        .artifacts(new DownloadItem()
                                                .allArtifacts(true)),
                                    new ScriptTask()
                                        .description("build, test, and publish")
                                        .location(ScriptTaskProperties.Location.FILE)
                                        .fileFromPath("bamboo_build.sh"),
                                    new ScriptTask()
                                        .description("test_fast")
                                        .enabled(false)
                                        .inlineBody("#!/bin/bash -xe\nexec bash repos/blasr/bamboo_test.sh"),
                                    new TestParserTask(TestParserTaskProperties.TestType.JUNIT)
                                        .description("run_cram_unit")
                                        .resultDirectories("build/*.xml"))
                                .requirements(new Requirement("system.dist")
                                        .matchValue("redhat")
                                        .matchType(Requirement.MatchType.EQUALS))))
                .linkedRepositories("blasr")
                
                .triggers(new BitbucketServerTrigger()
                        .name("Bitbucket Server repository triggered"),
                    new RemoteTrigger()
                        .name("Remote trigger"),
                    new ScheduledTrigger()
                        .name("Scheduled")
                        .description("nightly")
                        .enabled(false))
                .planBranchManagement(new PlanBranchManagement()
                        .createForPullRequest()
                        .delete(new BranchCleanup()
                            .whenRemovedFromRepositoryAfterDays(7)
                            .whenInactiveInRepositoryAfterDays(30))
                        .notificationForCommitters());
        
        bambooServer.publish(rootObject);
    }
}
