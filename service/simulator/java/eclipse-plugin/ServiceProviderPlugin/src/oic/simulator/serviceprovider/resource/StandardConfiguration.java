package oic.simulator.serviceprovider.resource;

import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import oic.simulator.serviceprovider.Activator;
import oic.simulator.serviceprovider.utils.Constants;

import org.eclipse.core.runtime.FileLocator;
import org.osgi.framework.Bundle;

public class StandardConfiguration {

    // A map of filename of standard resources as the key and the complete
    // location of the file(including the filename) as the value
    Map<String, String> stdConfigFiles;

    public StandardConfiguration() {
        stdConfigFiles = new HashMap<String, String>();
        addFiles();
        populateStandardConfigurationList();
    }

    private void addFiles() {
        stdConfigFiles.put(Constants.RAML_FILE_PREFIX + "light.raml", null);
    }

    private void populateStandardConfigurationList() {
        if (stdConfigFiles.isEmpty()) {
            return;
        }
        Iterator<String> fileItr = stdConfigFiles.keySet().iterator();
        String fileName;
        URL url;
        File file;
        String filePath;
        Bundle bundle = Activator.getDefault().getBundle();
        while (fileItr.hasNext()) {
            fileName = fileItr.next();
            url = bundle.getEntry(Constants.CONFIG_DIRECTORY_PATH + "/"
                    + fileName);
            if (null == url) {
                return;
            }
            System.out.println("URL: " + url.toExternalForm());
            try {
                file = new File(FileLocator.resolve(url).toURI());
            } catch (URISyntaxException | IOException e) {
                System.out.println(e.getMessage());
                return;
            }
            filePath = file.getAbsolutePath();
            System.out.println(filePath);
            stdConfigFiles.put(fileName, filePath);
        }

        // Print for debugging purpose
        Iterator<String> itr = stdConfigFiles.keySet().iterator();
        String key;
        while (itr.hasNext()) {
            key = itr.next();
            System.out.println("FileName: " + key);
            System.out.println("FilePath:" + stdConfigFiles.get(key));
        }
    }

    public Map<String, String> getStandardResourceConfigurationList() {
        return stdConfigFiles;
    }

    public void setStandardResourceConfigurationList(
            Map<String, String> stdConfigFiles) {
        this.stdConfigFiles = stdConfigFiles;
    }

    public String getFilePath(String fileName) {
        return stdConfigFiles.get(fileName);
    }
}