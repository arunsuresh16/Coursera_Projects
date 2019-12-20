import java.io.*;
import java.net.*;
import java.io.File;


class TCPServer {

  private static final String folderName = "encrypt_data";

  private static final String fileName =  "encrypted_file";
  private static int FileNum = 0;
  private static int NumOfFiles = 25;
  private static boolean FolderCreated = false;

  public static void main(String argv[]) throws Exception {
    String clientSentence;
    String capitalizedSentence;
    if (argv.length == 0)
    {
      System.out.println("Provide the port number as one of the command argument");
    }
    else if(argv.length > 1)
    {
      System.out.println("More than one arguments provided");
    }
    else
    {
      int port = Integer.parseInt(argv[0]);
      ServerSocket welcomeSocket = new ServerSocket(port);
      System.out.println("Starting the server and listening to port " + port);
      while (FileNum != NumOfFiles)
      {
         Socket connectionSocket = welcomeSocket.accept();
         BufferedReader inFromClient = new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
         clientSentence = inFromClient.readLine();
         System.out.println("Received: bytes " + clientSentence.length());
         FileNum++;
         createFile(clientSentence);
      }
      System.out.println("Done with " + FileNum + " files");
    }
  }

  public static void createFile(String data)
  {
    String fullFileName =  folderName + "//" + fileName + String.valueOf(FileNum) + ".txt";
    if(!FolderCreated)
    {
      System.out.println("Creating folder named " + folderName);

      File newFolder = new File(folderName);
        
      boolean created =  newFolder.mkdir();
        
      if(created)
      {
        System.out.println("Folder was created !");
      }
      else
      {
        System.out.println("Unable to create folder or folder is already created");
      }
      FolderCreated = true;
    }
    System.out.println("Creating File - " + fileName + String.valueOf(FileNum) + ".txt");
    try {
            FileWriter writer = new FileWriter(fullFileName, false);
            writer.write(data);
            writer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
  }
}