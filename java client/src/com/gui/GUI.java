package com.gui;

import javafx.scene.chart.PieChart;
import javafx.scene.layout.GridPane;

import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;
import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import sun.audio.AudioPlayer;
import sun.audio.AudioStream;

/**
 * Created by mebo on 11/7/16.
 */
public class GUI {
    private Container contain;
    private JPanel panel1;
    private JComboBox typeCombo;
    private JComboBox subjectCombo;
    private JButton getTestButton;
    private JPanel MainPanel;
    private JPanel QuestionAnswerPanel;
    private JLabel imageLabel;
    private JLabel Header;
    private JPanel imagePanel;
    private JPanel audioPanel;
    private JButton playAudioButton;
    private JButton stopAudioButton;
    private JTextArea answerTextArea;
    private JPanel videoPanel;
    private String subject;
    private String type;
    private Clip clip;
    private String fileName;
    private BufferedReader inFromUser;
    ImageIcon image;
    DatagramSocket clientSocket;
    InetAddress IPAddress;
    public GUI() {
        QuestionAnswerPanel.setVisible(false);
        QuestionAnswerPanel.setEnabled(false);
        imagePanel.setVisible(false);
        imagePanel.setEnabled(false);
        audioPanel.setVisible(false);
        audioPanel.setEnabled(false);
        inFromUser=new BufferedReader(new InputStreamReader(System.in));
        clientSocket = null;
        try {
            clientSocket = new DatagramSocket();
        } catch (SocketException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        IPAddress = null;
        try {
            IPAddress = InetAddress.getByName("localhost");
        } catch (UnknownHostException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        getTestButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {

                subject = subjectCombo.getSelectedItem().toString();
                type = typeCombo.getSelectedItem().toString();
                communicateWithServer();
            }
        });
        playAudioButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                startPlay();;
            }
        });
        stopAudioButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                stopPlay();
            }
        });
    }
    public void communicateWithServer() {
        byte[] sendData = new byte[1024];
        byte[] receiveData = new byte[1024];
        String sentence=subject+" "+type+'\0';
        sendData = sentence.getBytes();
        DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, 7891);
        try {
            clientSocket.send(sendPacket);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
        try {
            clientSocket.receive(receivePacket);
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        String modifiedSentence = new String(receivePacket.getData());

        if(modifiedSentence.contains("404")){
            JOptionPane.showMessageDialog(null,"Server Reply:" + "Requested test could not be found");

        }
        else{
            String[]address = modifiedSentence.split(" ");
            String tcpSentence = null;
            //  String tcpModifiedSentence;
            BufferedReader tcpInFromUser = new BufferedReader( new InputStreamReader(System.in));
            Socket tcpClientSocket = null;
            try {
                tcpClientSocket = new Socket(address[0], Integer.parseInt(address[1].replaceAll("\\D+","")));
            } catch (NumberFormatException e7) {
                // TODO Auto-generated catch block
                e7.printStackTrace();
            } catch (UnknownHostException e7) {
                // TODO Auto-generated catch block
                e7.printStackTrace();
            } catch (IOException e7) {
                // TODO Auto-generated catch block
                e7.printStackTrace();
            }
            DataOutputStream outToServer = null;
            try {
                outToServer = new DataOutputStream(tcpClientSocket.getOutputStream());
            } catch (IOException e7) {
                // TODO Auto-generated catch block
                e7.printStackTrace();
            }
            //BufferedReader inFromServer = new BufferedReader(new InputStreamReader(tcpClientSocket.getInputStream()));
            tcpSentence="Connect";
            try {
                outToServer.writeBytes(tcpSentence + '\n');
            } catch (IOException e5) {
                // TODO Auto-generated catch block
                e5.printStackTrace();
            }
            //tcpModifiedSentence = inFromServer.readLine();
            //byte[] b = tcpModifiedSentence.getBytes();
            //System.out.println("FROM SERVER: " + tcpModifiedSentence);
            // convert byte array back to BufferedImage
            DataInputStream in = null;
            try {
                in = new DataInputStream(tcpClientSocket.getInputStream());
            } catch (IOException e4) {
                // TODO Auto-generated catch block
                e4.printStackTrace();
            }
            MainPanel.removeAll();
            MainPanel.setVisible(false);
            System.out.print("Making Transition");
            QuestionAnswerPanel.setVisible(true);
            QuestionAnswerPanel.setEnabled(true);

            String finalMessage=null;
            String path="/home/mebo/IdeaProjects/Online Test System/src/com/gui/";
            if(type.equals("Image")){
                fileName="Image1.jpg";
                finalMessage="image transfer done";
            }
            else if(type.equals("Audio")){
                fileName="Audio1.wav";
                finalMessage="audio transfer done";
            }
            else if(type.equals("Video")){
                fileName="Video1.mp4";
                finalMessage="video transfer done";
            }

            File file = new File(path+fileName);
            try {
                file.createNewFile();
            } catch (IOException e3) {
                // TODO Auto-generated catch block
                e3.printStackTrace();
            }
            DataOutputStream dos = null;
            try {
                dos = new DataOutputStream(new FileOutputStream(file));
            } catch (FileNotFoundException e3) {
                // TODO Auto-generated catch block
                e3.printStackTrace();
            }
            int count;
            byte[] buffer = new byte[8192];
            int total_bytes=0;
            int fileSize=Integer.parseInt(address[2].replaceAll("\\D+",""));
            //;
            try {
                while (in.available()>0)
                {
                    count=in.read(buffer);
                    dos.write(buffer, 0, count);
                    dos.flush();
                    total_bytes+=count;
                }
            } catch (IOException e2) {
                // TODO Auto-generated catch block
                e2.printStackTrace();
            }
            try {
                dos.close();
            } catch (IOException e1) {
                // TODO Auto-generated catch block
                e1.printStackTrace();
            }
            if(type.equals("Image")) {
                imagePanel.setVisible(true);
                imagePanel.setEnabled(true);
                image = new ImageIcon(getClass().getResource("Image1.jpg"));
                imageLabel.setIcon(image);
                Header.setText("Provide Answer for the following image");

            }
            else if(type.equals("Audio")){
                audioPanel.setVisible(true);
                audioPanel.setEnabled(true);
                Header.setText("Provide Answer after listening to audio");


            }
            //JOptionPane.showMessageDialog(null,finalMessage);
            try {
                tcpClientSocket.close();
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }

        clientSocket.close();

    }

    public static void main(String[] args) {
        JFrame frame=new JFrame("Online Test System");
        frame.setSize(500,400);
        frame.setContentPane(new GUI().panel1);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        //frame.pack();
        frame.setVisible(true);

    }
    private void stopPlay() {
        if (clip != null) {
            clip.stop();
            clip.close();
            clip = null;
        }
    }
    private void startPlay(){
        try {
            stopPlay();
            AudioInputStream inputStream = AudioSystem
                    .getAudioInputStream(getClass().getResourceAsStream(fileName));
            clip = AudioSystem.getClip();
            clip.open(inputStream);
            clip.start();
        } catch (Exception e) {
            stopPlay();
            System.err.println(e.getMessage());
        }
    }



}
