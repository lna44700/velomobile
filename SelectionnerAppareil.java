package com.example.silver.vmapp;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import java.util.ArrayList;
import java.util.List;

/**
 * Déclaration des objets
 */

public class SelectionnerAppareil extends Activity {
    private Bluetooth bluetoothClasse;
    private ListView vuListe;
    private List<BluetoothDevice> listeAppareilAssocie;
    private boolean enregistre = false;

    /**
     * @fnonCreate
     * Déclaration des méthodes et crée les liaisons entre les fichiers JAVA et XML
     * @param sauvegarderEtat
     * Crée une copie de Bundle (ordonne les chaines de caractères)
     */

    @Override
    protected void onCreate(Bundle sauvegarderEtat) {
        super.onCreate(sauvegarderEtat);
        setContentView(R.layout.fenetre_selectionner_appareil);

        IntentFilter filter = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
        registerReceiver(Recevoir, filter);
        enregistre = true;

        bluetoothClasse = new Bluetooth(this);
        bluetoothClasse.activerBluetooth();

        vuListe = (ListView) findViewById(R.id.liste_appareils);

        vuListe.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                Intent i = new Intent(SelectionnerAppareil.this, Messages.class);
                i.putExtra("pos", position);
                if (enregistre) {
                    unregisterReceiver(Recevoir);
                    enregistre = false;
                }
                startActivity(i);
                finish();
            }
        });

        ajouterAppareilsListe();
    }

    /**
     * @fnonDestroy
     * Destruction de la variable reçue
     */

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (enregistre) {
            unregisterReceiver(Recevoir);
            enregistre = false;
        }
    }

    /**
     * @fnajouterAppareilsListe
     * Ajoute les appareils dans une liste pour qu'on puisse les séléctionner
     */

    private void ajouterAppareilsListe() {
        listeAppareilAssocie = bluetoothClasse.getAppareilsAssocies();

        List<String> noms = new ArrayList<>();
        for (BluetoothDevice d : listeAppareilAssocie) {
            noms.add(d.getName());
        }

        String[] array = noms.toArray(new String[noms.size()]);

        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, android.R.id.text1, array);

        vuListe.setAdapter(adapter);
    }

    /**
     * @fnBroadcastReceiver
     * Reçoit les apparails associés
     */

    private final BroadcastReceiver Recevoir = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();

            if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);

                switch (state) {
                    case BluetoothAdapter.STATE_OFF:
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                vuListe.setEnabled(false);
                            }
                        });
                        break;
                    case BluetoothAdapter.STATE_ON:
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                ajouterAppareilsListe();
                                vuListe.setEnabled(true);
                            }
                        });
                        break;
                }
            }
        }
    };
}