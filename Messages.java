package com.example.silver.vmapp;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.text.method.ScrollingMovementMethod;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
//import android.widget.ScrollView;
import android.widget.TextView;


/**
 * Déclaration des objets
 */


public class Messages extends AppCompatActivity implements Bluetooth.CommunicationCallback {
    private String nom;
    private Bluetooth bluetoothClasse;
    private TextView text;
    //private ScrollView vueDefilee;
    private boolean enregistre=false;

    /**
     * @fnonCreate
     * Déclaration des méthodes et crée les liaisons entre les fichiers JAVA et XML
     * @param sauvegarderEtat
     * Crée une copie de Bundle (ordonne les chaines de caractères)
     */

    @Override
    protected void onCreate(Bundle sauvegarderEtat) {
        super.onCreate(sauvegarderEtat);
        setContentView(R.layout.fenetre_message);

        text = (TextView)findViewById(R.id.messages);


        //vueDefilee = (ScrollView) findViewById(R.id.defiler);

        text.setMovementMethod(new ScrollingMovementMethod());

        bluetoothClasse = new Bluetooth(this);
        bluetoothClasse.activerBluetooth();

        bluetoothClasse.setCommunicationRetour(this);

        int pos = getIntent().getExtras().getInt("pos");
        nom = bluetoothClasse.getAppareilsAssocies().get(pos).getName();

        //Afficher("Connexion...");
        bluetoothClasse.connecterAAppareil(bluetoothClasse.getAppareilsAssocies().get(pos));

        IntentFilter filter = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(Recevoir, filter);
        enregistre=true;
    }

    /**
     * @fnonDestroy
     * Destruction de la variable reçue
     */

    @Override
    public void onDestroy() {
        super.onDestroy();
        if(enregistre) {
            unregisterReceiver(Recevoir);
            enregistre=false;
        }
    }

    /**
     * @fnonCreateOptionsMenu
     * Crée le menu dans l'application
     */

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater MenuDefiler = getMenuInflater();
        MenuDefiler.inflate(R.menu.menu, menu);
        return true;
    }

    /**
     * @fnonOptionsItemSelected
     * Lorsqu'on appuie sur Deconnecter, on arrête la communication et bascule sur la fenêtre principale
     * @param option
     * @return
     */

    @Override
    public boolean onOptionsItemSelected(MenuItem option) {
        switch (option.getItemId()) {
            case R.id.ferme:
                //bluetoothClasse.supCommunicationRetour();
                bluetoothClasse.deconnecter();
                Intent nouvelleFenetre = new Intent(this, SelectionnerAppareil.class);
                startActivity(nouvelleFenetre);
                finish();
                return true;
            default:
                return super.onOptionsItemSelected(option);
        }
    }

    /**
     * @fnAfficher
     * Afficher les valeurs reçues
     * @param m
     * Stock dans la variable m et l'affiche l'une après l'autre grâce à \n
     */

    public void Afficher(final String m){
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                text.append(m + "\n");
                //vueDefilee.fullScroll(View.FOCUS_DOWN);
            }
        });
    }

    /**
     * Affiche que l'appareil est connecter vers telle appareils (affiche le nom et l'adresse MAC)
     */

    @Override
    public void onConnecte(BluetoothDevice appareil) {
        //Afficher("Connecté à "+appareil.getName()+" - "+appareil.getAddress());
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {

            }
        });
    }

    @Override
    public void onDisconnecte(BluetoothDevice appareil, String message) {
//        Afficher("Déconnecté");
//        Afficher("Re-connexion...");
        bluetoothClasse.connecterAAppareil(appareil);
    }

    /**
     * @fnonMessage
     * Affiche le nom de l'appareil qui envoie les données, ainsi que les messages
     */

    @Override
    public void onMessage(String message) {

//        String Tension = message.substring(0, message.indexOf("\\", 0));

//        Afficher("Tension = " + Tension + "V");

//               Afficher(nom+": "+message);

//        int finTension   = message.indexOf("V",0);
//        int finIntensite = message.indexOf('A',0);
//        int finPuissance = message.indexOf("W",0);
//        int finVitesse   = message.indexOf("S",0);
//        int finDistance  = message.indexOf("D",0);
//        int finCapacite  = message.indexOf("C",0);
//
//        String Tension   = message.substring(0, finTension);
//        String Intensite = message.substring(finTension+1, finIntensite);
//        String Puissance = message.substring(finIntensite+1, finPuissance);
//        String Vitesse   = message.substring(finPuissance+1, finVitesse);
//        String Distance  = message.substring(finVitesse+1, finDistance);
//        String Capacite  = message.substring(finDistance+1, finCapacite);
//
//        String Final = "";
//        Final = Final.concat(Tension + Intensite + Puissance + Vitesse + Distance + Capacite);
//        Afficher(Final);

      Afficher(message);
    }
    @Override
    public void onErreur(String message) {
        Afficher("Erreur: "+message);
    }

    /**
     * @fnonErreurConnexion
     * En cas de l'erreur de connexion afficher les messages...
     * @param appareil
     * @param message
     */

    @Override
    public void onErreurConnexion(final BluetoothDevice appareil, String message) {
//        Afficher("Erreur de connexion...");
//        Afficher("Tentative de connexion...");
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Handler handler = new Handler();
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        bluetoothClasse.connecterAAppareil(appareil);
                    }
                }, 2000);
            }
        });
    }

    /**
     * @BroadcastReceiver
     * Permet de recevoir les données venant des autre appareils
     */

    private final BroadcastReceiver Recevoir = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                final int etat = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
                Intent intent1 = new Intent(Messages.this, SelectionnerAppareil.class);

                switch (etat) {
                    case BluetoothAdapter.STATE_OFF:
                        if(enregistre) {
                            unregisterReceiver(Recevoir);
                            enregistre=false;
                        }
                        startActivity(intent1);
                        finish();
                        break;
                    case BluetoothAdapter.STATE_TURNING_OFF:
                        if(enregistre) {
                            unregisterReceiver(Recevoir);
                            enregistre=false;
                        }
                        startActivity(intent1);
                        finish();
                        break;
                }
            }
        }
    };
}