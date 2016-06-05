package com.example.silver.vmapp;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

/**
 * Déclaration des objets
 */


public class Bluetooth {
    private static final UUID MON_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");
    private BluetoothAdapter bluetoothAdaptateur;
    private BluetoothSocket socket;
    private BluetoothDevice appareil;
    private BufferedReader messageEntrant;
    private OutputStream sortie;

    private boolean connecte=false;
    private CommunicationCallback communication=null;

    private Activity fenetre;

    /**
     * @fnBluetooth
     * Savoir si l'appareil possède le Bluetooth ou pas
     * @param fenetre_principale
     * Montre où l'action se passe, sur quelle fenêtre
     */

    public Bluetooth(Activity fenetre_principale){
        this.fenetre=fenetre_principale;
        bluetoothAdaptateur = BluetoothAdapter.getDefaultAdapter();
    }

    /**
     * @fnacticerBluetooth
     * Si le Bluetooth est présent sur l'appareil, activer le Bluetooth
     */

    public void activerBluetooth(){
        if(bluetoothAdaptateur!=null) {
            if (!bluetoothAdaptateur.isEnabled()) {
                bluetoothAdaptateur.enable();
            }
        }
    }

    /**
     * @fndesactiverBluetooth
     * Si le Bluetooth est présent sur l'appareil, désactiver le Bluetooth
     */

    public void desactiverBluetooth(){
        if(bluetoothAdaptateur!=null) {
            if (bluetoothAdaptateur.isEnabled()) {
                bluetoothAdaptateur.disable();
            }
        }
    }

    /**
     * @fnconnecterALAdresse
     * L'appareil se connecte grâce à l'adresse MAC
     * @param adresse
     * Récupère l'adresse MAC
     */

    public void connecterALAdresse(String adresse) {
        BluetoothDevice appareil = bluetoothAdaptateur.getRemoteDevice(adresse);
        new ConnectThread(appareil).start();
    }

    /**
     * @fnconnecterAuNom
     * Récupère les appareils associés au téléphone
     * @param nom
     * Récupère le nom des appareils associés
     */

    public void connecterAuNom(String nom) {
        for (BluetoothDevice appareilBluetooth : bluetoothAdaptateur.getBondedDevices()) {
            if (appareilBluetooth.getName().equals(nom)) {
                connecterALAdresse(appareilBluetooth.getAddress());
            }
        }
    }

    /**
     * @fnconnecterAAppareil
     * Le téléphone utilise la socket pour se connecter
     * @param appareil
     * Récupère l'appareil pour se connecter
     */

    public void connecterAAppareil(BluetoothDevice appareil){
        new ConnectThread(appareil).start();
    }

    /**
     * @fndeconnecter
     * Ferme les sockets et arrete la communication entre les appareils
     */

    public void deconnecter() {
        try {
            socket.close();
        } catch (IOException e) {
            if(communication!=null)
                communication.onErreur(e.getMessage());
        }
    }

    public boolean estConnecte(){
        return connecte;
    }

    /**
     * @fnReceiveThread
     * Récupère la valeur reçue et la lit à travers la communication entre les appareils
     */

    private class ReceiveThread extends Thread implements Runnable{
        public void run(){
            String msg;
            try {
               while ((msg = messageEntrant.readLine()) != null) {
                    if (communication != null)
                        communication.onMessage(msg);
                }
            } catch (IOException e) {
                connecte=false;
                if (communication != null)
                    communication.onDisconnecte(appareil, e.getMessage());
            }
        }
    }

    /**
     * @fnConnecteThread
     * Se connecter vers l'appareil avec l'ID unique en utilisant les sockets
     */

    private class ConnectThread extends Thread {
        public ConnectThread(BluetoothDevice appareil) {
            Bluetooth.this.appareil=appareil;
            try {
                Bluetooth.this.socket = appareil.createRfcommSocketToServiceRecord(MON_UUID);
            } catch (IOException e) {
                if(communication!=null)
                    communication.onErreur(e.getMessage());
            }
        }

        /**
         * @fnrun
         * Se connecter, récupèrer les valuers réçues, les traduires de octet en caractère
         */

        public void run() {

            try {
                socket.connect();
                sortie = socket.getOutputStream();
                messageEntrant = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                connecte=true;

                new ReceiveThread().start();

                if(communication!=null)
                    communication.onConnecte(appareil);
            } catch (IOException e) {
                if(communication!=null)
                    communication.onErreurConnexion(appareil, e.getMessage());

                try {
                    socket.close();
                } catch (IOException closeException) {
                    if (communication != null)
                        communication.onErreur(closeException.getMessage());
                }
            }
        }
    }

    /**
     * @fngetAppareilsAssocies
     * Récupérer les appareils pour les mettre dans la liste
     * @return
     */

    public List<BluetoothDevice> getAppareilsAssocies(){
        List<BluetoothDevice> appareils = new ArrayList<>();
        for (BluetoothDevice appareilBluetooth : bluetoothAdaptateur.getBondedDevices()) {
            appareils.add(appareilBluetooth);
        }
        return appareils;
    }

    public BluetoothSocket getSocket(){
        return socket;
    }

    public BluetoothDevice getDevice(){
        return appareil;
    }

    /**
     * @fnCommunicationCallBack
     * Utilise toutes les fonctions pour la communication
     */

    interface CommunicationCallback{
        void onConnecte(BluetoothDevice appareil);
        void onDisconnecte(BluetoothDevice appareil, String message);
        void onMessage(String message);
        void onErreur(String message);
        void onErreurConnexion(BluetoothDevice appareil, String message);
    }

    public void setCommunicationRetour(CommunicationCallback communicationRetour) {
        this.communication = communicationRetour;
    }
//
//    public void supCommunicationRetour(){
//        this.communication = null;
//    }


}
